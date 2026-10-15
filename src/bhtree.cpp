#include <nbody/bhtree.hpp>
#include <cmath>
#include <algorithm>
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
}

void BHTree::insert(std::size_t body_idx, std::size_t node_idx, std::span<const Body> bodies){
    if (nodes_[node_idx].is_empty()) {
        nodes_[node_idx].mass = bodies[body_idx].mass;
        nodes_[node_idx].weighted_mass_positon = bodies[body_idx].position * bodies[body_idx].mass;
        nodes_[node_idx].body_idx = body_idx;
        return;
    }

    if (nodes_[node_idx].is_internal()){
        return move_body_down(body_idx, node_idx, bodies);
    }

    //encountered an external body (leaf), need to make current node an internal node and move both children down
    size_t leaf_body_idx = nodes_[node_idx].body_idx;
    nodes_[node_idx].mass = 0;
    nodes_[node_idx].weighted_mass_positon = {};
    move_body_down(leaf_body_idx, node_idx, bodies);
    move_body_down(body_idx, node_idx, bodies);
}

void BHTree::move_body_down(std::size_t body_idx, std::size_t parent_idx, std::span<const Body> bodies){
    Body b = bodies[body_idx];
    nodes_[parent_idx].body_idx = -1;
    nodes_[parent_idx].mass += b.mass;
    nodes_[parent_idx].weighted_mass_positon += b.position * b.mass;
    int octant = nodes_[parent_idx].cube.octant_for(b.position);
    if (nodes_[parent_idx].children[octant] == no_node) {
        CubeQuadrant cq = nodes_[parent_idx].cube.child_quadrant(b.position);
        nodes_.push_back(BHNode{cq});
        nodes_[parent_idx].children[octant] = nodes_.size() - 1;
    } else {
        if (nodes_[nodes_[parent_idx].children[octant]].cube.half_width < COLLISION_HALF_WIDTH){
            nodes_[nodes_[parent_idx].children[octant]].mass += b.mass;
            nodes_[nodes_[parent_idx].children[octant]].weighted_mass_positon += b.position * b.mass;
            return;
        }
    }
    return insert(body_idx, nodes_[parent_idx].children[octant], bodies);
}

void BHTree::calculate(std::span<const Body> bodies, std::span<Vec3> accelerations){
    build(bodies);
    for (size_t i = 0; i < bodies.size(); i++){
        accelerations[i] = accel_from(nodes_[0], i, bodies);
    }
}

Vec3 BHTree::accel_from(BHNode node, std::size_t body_idx, std::span<const Body> bodies){
    if (node.is_empty()) return{};
    if (node.body_idx == body_idx) return{};
    Vec3 r = (node.weighted_mass_positon / node.mass) - bodies[body_idx].position; //remember a leaf can be combined bodies
    float r_mag = r.magnitude();
    if (node.is_leaf()){ 
        float r_squared_softened = r_mag * r_mag + SOFTENING_LENGTH * SOFTENING_LENGTH;
        return r * (G * node.mass / (std::pow(r_squared_softened, 1.5)));
    }
    float s = node.cube.half_width * 2;
    if (s/r_mag < theta_){
        float r_squared_softened = r_mag * r_mag + SOFTENING_LENGTH * SOFTENING_LENGTH;
        return r * (G * node.mass / (std::pow(r_squared_softened, 1.5)));
    }
    Vec3 accel{};
    for (size_t i = 0; i < node.children.size(); i++){
        if (node.children[i] == no_node) continue;
        accel += accel_from(nodes_[node.children[i]], body_idx, bodies);
    }
    return accel;
}