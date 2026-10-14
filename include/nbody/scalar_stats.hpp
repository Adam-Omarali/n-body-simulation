#ifndef NBODY_SCALARSTATS_HPP
#define NBODY_SCALARSTATS_HPP

#include <cstddef>
#include <algorithm>
#include <cmath>

struct ScalarStats{
    private:
        float initial_ = 0;
        float current_ = 0;
        float max_relative_err_ = 0;
        float current_err_ = 0;
        float sum_squared_err_ = 0;
        std::size_t n_samples_ = 0;

    public:
        void observe(float sample){
            if (n_samples_ == 0) {
                initial_ = current_ = sample;
                n_samples_ = 1;
                return;
            }
            current_ = sample;
            current_err_ = std::abs(current_ - initial_);
            if (initial_ == 0) {
                max_relative_err_ = std::max(current_err_, max_relative_err_);
            } else {
                max_relative_err_ = std::max(current_err_ / std::abs(initial_), max_relative_err_);
            }
            sum_squared_err_ += current_err_ * current_err_;
            n_samples_ += 1;
        };

        float current_error() const { return current_err_; }
        float max_relative_error() const { return max_relative_err_; }
};

#endif
