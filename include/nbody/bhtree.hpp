#ifndef NBODY_BHTREE_HPP
#define NBODY_BHTREE_HPP

#include <nbody/cube_quadrant.hpp>
#include <nbody/body.hpp>
#include <span>

static constexpr std::uint32_t no_node = std::numeric_limits<std::uint32_t>::max();
static constexpr std::uint32_t no_body = std::numeric_limits<std::uint32_t>::max() - 1;

struct BHTree {
    public:
        BHTree(float theta): theta_(theta){};
        void build(std::span<const Body> bodies);
        void insert(std::uint32_t body_idx, std::uint32_t node_idx, std::span<const Body> bodies);
        void calculate(std::span<const Body> bodies, std::span<Vec3> accelerations);
    private:
        struct BHNode {
            CubeQuadrant cube;
            float mass = 0;
            Vec3 centre_of_mass{};
            std::uint32_t body_idx = no_body;
            uint32_t first_child = no_node;

            BHNode(CubeQuadrant c):cube(c){}
            bool is_empty() const{
                if (body_idx != no_body) return false;
                if (first_child != no_node) return false;
                return true;
            }
            bool is_internal() const{
                return !is_empty() && body_idx == no_body;
            }
            bool is_leaf() const{
                return !is_empty() && body_idx != no_body;
            }
        };
        float theta_;
        std::vector<BHNode> nodes_;
        void move_body_down(std::uint32_t body_idx, std::uint32_t parent_idx, std::span<const Body> bodies);        
        Vec3 accel_from(std::uint32_t node_idx, std::uint32_t body_idx, std::span<const Body> bodies);
};

#endif