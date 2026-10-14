#ifndef NBODY_VECTORSTATS_HPP
#define NBODY_VECTORSTATS_HPP

#include <nbody/vec3.hpp>
#include <cstddef>
#include <algorithm>

struct VectorStats{
    private:
        Vec3 initial_{};
        Vec3 current_{};
        float max_relative_err_ = 0;
        float current_err_ = 0;
        float squared_err_ = 0;
        std::size_t n_samples_ = 0;

    public:
        void observe(const Vec3& sample){
            if (n_samples_ == 0) {
                initial_ = current_ = sample;
                n_samples_ = 1;
                return;
            }
            current_ = sample;
            current_err_ = (sample - initial_).magnitude();
            if (initial_.magnitude() == 0){
                max_relative_err_ = std::max(current_.magnitude(), max_relative_err_);
            } else{
                max_relative_err_ = std::max(current_err_ / initial_.magnitude(), max_relative_err_);
            }
            squared_err_ += current_err_ * current_err_;
            n_samples_ += 1;
        };

        float current_error() const { return current_err_; }
        float max_relative_error() const { return max_relative_err_; }
};

#endif
