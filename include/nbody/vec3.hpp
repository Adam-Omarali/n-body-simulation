#ifndef NBODY_VEC3_HPP
#define NBODY_VEC3_HPP

#include <cmath>
#include <raylib.h>
#include <iostream>

struct Vec3 {
    float x;
    float y;
    float z;

    Vec3 operator+(Vec3 other) const{
        return {
            x + other.x,
            y + other.y,
            z + other.z
        };
    };
    Vec3 operator+=(Vec3 other){
        x = x + other.x;
        y = y + other.y;
        z = z + other.z;
        return {x, y, z};
    };
    Vec3 operator-(Vec3 other) const{
        return {
            x - other.x,
            y - other.y,
            z - other.z
        };
    };
    Vec3 operator*(float scale) const{
        return {
            x * scale,
            y * scale,
            z * scale
        };
    };
    Vec3 operator/(float scale) const{
        return {
            x / scale,
            y / scale,
            z / scale
        };
    };
    float magnitude() const{
        return sqrt(x * x + y * y + z * z);
    }
    Vector3 to_raylib_vec3() const{
        return Vector3(x, y, z);
    }
    Vec3 normalize() const {
        float mag = magnitude();
        return {x / mag, y / mag, z / mag};
    }
    void print() const {
        std::cout<<"x: "<<x<<" y: "<<y<<" z: "<<z<<'\n';
    }
};

inline float dot(const Vec3& v1, const Vec3& v2){
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

inline Vec3 cross(const Vec3& v1, const Vec3& v2){
    return {
        v1.y * v2.z - v2.y * v1.z, 
        -(v1.x * v2.z - v2.x * v1.z),
        v1.x * v2.y - v2.x * v2.y
    };
}

#endif