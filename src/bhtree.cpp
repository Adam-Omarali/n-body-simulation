#include <nbody/bhtree.hpp>
#include <cmath>
#include <algorithm>
#include <atomic>
#include <thread>
#include <nbody/constants.hpp>

float MINIMUM_HALF_WIDTH = 1;
float COLLISION_HALF_WIDTH = 0.1;


void BHTree::build(std::span<const Body> bodies){
    nodes_.clear();
    if (bodies.size() == 0) return;
    Vec3 max_coords{bodies[0].position};
    Vec3 min_coords{bodies[0].position};
    for (const Body& b : bodies){
        if (b.position.x > max_coords.x) max_coords.x = b.position.x;
        if (b.position.y > max_coords.y) max_coords.y = b.position.y;
        if (b.position.z > max_coords.z) max_coords.z = b.position.z;

        if (b.position.x < min_coords.x) min_coords.x = b.position.x;
        if (b.position.y < min_coords.y) min_coords.y = b.position.y;
        if (b.position.z < min_coords.z) min_coords.z = b.position.z;
    }

    Vec3 centre{(max_coords + min_coords) / 2};
    Vec3 diff = max_coords - min_coords;
    float half_width = 0.5f * std::max(diff.x, std::max(diff.y, diff.z)) * 1.001f;
    half_width = std::max(half_width, MINIMUM_HALF_WIDTH);

    nodes_.reserve(bodies.size() * 2);
    nodes_.push_back(BHNode{CubeQuadrant{centre, half_width}});
    for (size_t i = 0; i < bodies.size(); i++){
        insert(i, 0, bodies);
    }
    for (size_t i = 0; i < nodes_.size(); i++){
        if (nodes_[i].is_empty()) continue;
        nodes_[i].centre_of_mass = nodes_[i].centre_of_mass / nodes_[i].mass;
    }
}

void BHTree::insert(std::uint32_t body_idx, std::uint32_t node_idx, std::span<const Body> bodies){
    if (nodes_[node_idx].is_empty()) {
        nodes_[node_idx].mass = bodies[body_idx].mass;
        nodes_[node_idx].centre_of_mass = bodies[body_idx].position * bodies[body_idx].mass;
        nodes_[node_idx].body_idx = body_idx;
        return;
    }

    if (nodes_[node_idx].is_internal()){
        return move_body_down(body_idx, node_idx, bodies);
    }

    //encountered an external body (leaf), need to make current node an internal node and move both children down
    size_t leaf_body_idx = nodes_[node_idx].body_idx;
    nodes_[node_idx].mass = 0;
    nodes_[node_idx].centre_of_mass = {};
    move_body_down(leaf_body_idx, node_idx, bodies);
    move_body_down(body_idx, node_idx, bodies);
}

void BHTree::move_body_down(std::uint32_t body_idx, std::uint32_t parent_idx, std::span<const Body> bodies){
    Body b = bodies[body_idx];
    nodes_[parent_idx].body_idx = no_body;
    nodes_[parent_idx].mass += b.mass;
    nodes_[parent_idx].centre_of_mass += b.position * b.mass;

    //create all children for contigious access and to only store first child pointer
    if (nodes_[parent_idx].first_child == no_node){
        nodes_[parent_idx].first_child = nodes_.size();
        for (uint8_t i = 0; i < 8; i++){
            CubeQuadrant cq = nodes_[parent_idx].cube.child_quadrant(i);
            nodes_.push_back(BHNode(cq));
        }
    }

    int octant = nodes_[parent_idx].cube.octant_for(b.position);
    if (!nodes_[nodes_[parent_idx].first_child + octant].is_empty() && 
        nodes_[nodes_[parent_idx].first_child + octant].cube.half_width < COLLISION_HALF_WIDTH){
        nodes_[nodes_[parent_idx].first_child + octant].mass += b.mass;
        nodes_[nodes_[parent_idx].first_child + octant].centre_of_mass += b.position * b.mass;
        return;
    }
    return insert(body_idx, nodes_[parent_idx].first_child + octant, bodies);
}

void BHTree::calculate(std::span<const Body> bodies, std::span<Vec3> accelerations){
    build(bodies);

    // tree is read-only from here and each body writes only accelerations[i], so bodies can run in parallel.
    // threads claim chunks from a shared counter since bodies in dense regions cost more to traverse.
    const std::size_t n = bodies.size();
    const std::size_t chunk = 128;
    unsigned num_threads = num_threads_ ? num_threads_ : std::max(1u, std::thread::hardware_concurrency());
    std::atomic<std::size_t> next{0};

    auto worker = [&](){
        while (true){
            std::size_t start = next.fetch_add(chunk);
            if (start >= n) break;
            std::size_t end = std::min(start + chunk, n);
            for (std::size_t i = start; i < end; i++){
                accelerations[i] = accel_from(0, i, bodies);
            }
        }
    };

    std::vector<std::thread> threads;
    for (unsigned t = 0; t < num_threads; t++){
        threads.emplace_back(worker);
    }
    for (std::thread& t : threads){
        t.join();
    }
}

Vec3 BHTree::accel_from(std::uint32_t node_idx, std::uint32_t body_idx, std::span<const Body> bodies) const{
    const BHNode& node = nodes_[node_idx];
    if (node.is_empty()) return{};
    if (node.body_idx == body_idx) return{};
    Vec3 r = node.centre_of_mass - bodies[body_idx].position; //remember a leaf can be combined bodies
    float r_mag = r.magnitude();
    if (node.is_leaf()){ 
        float r_squared_softened = r_mag * r_mag + SOFTENING_LENGTH * SOFTENING_LENGTH;
        return r * (G * node.mass / (r_squared_softened * sqrt(r_squared_softened)));
    }
    float s = node.cube.half_width * 2;
    if (s/r_mag < theta_){
        float r_squared_softened = r_mag * r_mag + SOFTENING_LENGTH * SOFTENING_LENGTH;
        return r * (G * node.mass / (r_squared_softened * sqrt(r_squared_softened)));
    }
    Vec3 accel{};
    for (size_t i = 0; i < 8; i++){
        accel += accel_from(node.first_child + i, body_idx, bodies);
    }
    return accel;
}