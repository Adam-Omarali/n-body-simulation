#ifndef NBODY_BHTREE_HPP
#define NBODY_BHTREE_HPP

#include <nbody/cube_quadrant.hpp>
#include <nbody/body.hpp>
#include <span>

static constexpr std::size_t no_node = std::numeric_limits<std::size_t>::max();
static constexpr std::size_t no_body = std::numeric_limits<std::size_t>::max() - 1;

struct BHTree {
    public:
        BHTree(float theta): theta_(theta){};
        void build(std::span<const Body> bodies);
        void insert(std::size_t body_idx, std::size_t node_idx, std::span<const Body> bodies);
        void calculate(std::span<const Body> bodies, std::span<Vec3> accelerations);
    private:
        struct BHNode {
            CubeQuadrant cube;
            float mass = 0;
            Vec3 weighted_mass_positon{};
            std::size_t body_idx = no_body;
            std::array<std::size_t, 8> children;

            BHNode(CubeQuadrant c):cube(c){
                children.fill(no_node);
            }
            bool is_empty(){
                if (body_idx != no_body) return false;
                for (size_t c: children){
                    if (c != no_node) return false;
                }
                return true;
            }
            bool is_internal(){
                return !is_empty() && body_idx == no_body;
            }
            bool is_leaf(){
                return !is_empty() && body_idx != no_body;
            }
        };
        float theta_;
        std::vector<BHNode> nodes_;
        void move_body_down(std::size_t body_idx, std::size_t parent_idx, std::span<const Body> bodies);
        Vec3 accel_from(BHNode node, std::size_t body_idx, std::span<const Body> bodies);
};

#endif