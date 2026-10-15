#ifndef NBODY_CUBEQUADRANT_HPP
#define NBODY_CUBEQUADRANT_HPP

#include <nbody/vec3.hpp>

struct CubeQuadrant{
    Vec3 centre;
    float half_width;
    bool contains(Vec3& p){
        return abs(p.x - centre.x) < half_width && abs(p.y - centre.y) < half_width && abs(p.z - centre.z) < half_width;
    }
    std::uint8_t octant_for(const Vec3& p) const{
        return (p.x > centre.x ? 1 : 0) | 
                (p.y > centre.y ? 1 : 0) << 1 | 
                (p.z > centre.z ? 1 : 0) << 2;
    }
    CubeQuadrant child_quadrant(const Vec3& p) const {
        int octant = octant_for(p);
        Vec3 new_centre{};
        float new_half_width = half_width / 2;
        if ((octant & 0b001) == 0){
            new_centre.x = centre.x - new_half_width;
        } else {
            new_centre.x = centre.x + new_half_width;
        }

        if ((octant & 0b010) == 0){
            new_centre.y = centre.y - new_half_width;
        } else {
            new_centre.y = centre.y + new_half_width;
        }

        if ((octant & 0b100) == 0){
            new_centre.z = centre.z - new_half_width;
        } else {
            new_centre.z = centre.z + new_half_width;
        }
        return {new_centre, new_half_width};
    }
};

#endif