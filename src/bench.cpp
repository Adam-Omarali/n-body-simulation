// Benchmarks DirectGravity vs BHTree force calculation.
//
//   ./build/bench                       sweep N and theta, write out/bench.csv
//   ./build/bench profile <N> <theta> <iters> [threads, 0 = all]
//                                       run only BHTree in a loop, for the profiler

#include <nbody/body.hpp>
#include <nbody/direct_gravity.hpp>
#include <nbody/bhtree.hpp>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

constexpr unsigned SEED = 42;
constexpr std::size_t REPEATS = 5;
constexpr std::size_t MAX_DIRECT_N = 20000; // brute force gets too slow past this

std::vector<Body> make_bodies(std::size_t n){
    std::mt19937 rng(SEED);
    // scale the box with N so density stays the same as main.cpp (200 bodies in +-2)
    float max_position = 2.0f * std::cbrt(n / 200.0f);
    std::vector<Body> bodies(n);
    for (Body& b : bodies) b = spawn_body(rng, max_position, 0.5f, 10.0f);
    return bodies;
}

// median wall time in milliseconds of calc.calculate(), after one warm-up call
template <typename ForceCalculator>
double time_ms(ForceCalculator& calc, std::span<const Body> bodies, std::span<Vec3> accel, std::size_t repeats){
    calc.calculate(bodies, accel);
    std::vector<double> times;
    for (std::size_t r = 0; r < repeats; r++){
        auto start = std::chrono::steady_clock::now();
        calc.calculate(bodies, accel);
        auto end = std::chrono::steady_clock::now();
        times.push_back(std::chrono::duration<double, std::milli>(end - start).count());
    }
    std::sort(times.begin(), times.end());
    return times[times.size() / 2];
}

// mean of |a - ref| / |ref| over all bodies
double mean_relative_error(std::span<const Vec3> a, std::span<const Vec3> ref){
    double total = 0;
    for (std::size_t i = 0; i < a.size(); i++){
        float ref_mag = ref[i].magnitude();
        if (ref_mag == 0) continue;
        total += (a[i] - ref[i]).magnitude() / ref_mag;
    }
    return total / a.size();
}

int sweep(){
    const std::vector<std::size_t> sizes{100, 500, 1000, 2000, 5000, 10000, 20000, 50000, 100000};
    const std::vector<float> thetas{0.3f, 0.5f, 0.8f};

    std::ofstream csv("out/bench.csv");
    csv << "n,solver,theta,median_ms,mean_rel_error\n";

    for (std::size_t n : sizes){
        std::vector<Body> bodies = make_bodies(n);
        std::vector<Vec3> direct_accel(n);
        bool have_direct = n <= MAX_DIRECT_N;

        if (have_direct){
            DirectGravity direct;
            std::size_t repeats = n >= 10000 ? 1 : REPEATS;
            double ms = time_ms(direct, bodies, direct_accel, repeats);
            csv << n << ",direct,," << ms << ",0\n";
            std::cout << "n=" << n << " direct " << ms << " ms\n";
        }

        for (float theta : thetas){
            BHTree tree(theta);
            std::vector<Vec3> bh_accel(n);
            double ms = time_ms(tree, bodies, bh_accel, REPEATS);
            csv << n << ",barnes_hut," << theta << "," << ms << ",";
            if (have_direct) csv << mean_relative_error(bh_accel, direct_accel);
            csv << "\n";
            std::cout << "n=" << n << " bh theta=" << theta << " " << ms << " ms";
            if (have_direct) std::cout << " err=" << mean_relative_error(bh_accel, direct_accel);
            std::cout << "\n";
        }
        csv.flush();
    }
    std::cout << "wrote out/bench.csv\n";
    return 0;
}

int profile(std::size_t n, float theta, std::size_t iters, unsigned threads){
    std::vector<Body> bodies = make_bodies(n);
    std::vector<Vec3> accel(n);
    BHTree tree(theta, threads);
    auto start = std::chrono::steady_clock::now();
    for (std::size_t i = 0; i < iters; i++) tree.calculate(bodies, accel);
    auto end = std::chrono::steady_clock::now();
    double ms = std::chrono::duration<double, std::milli>(end - start).count();
    std::cout << "n=" << n << " theta=" << theta << " threads=" << (threads ? std::to_string(threads) : "all")
              << " " << ms / iters << " ms/iter\n";
    return 0;
}

int main(int argc, char** argv){
    if (argc >= 2 && std::string(argv[1]) == "profile"){
        std::size_t n = argc > 2 ? std::stoul(argv[2]) : 50000;
        float theta = argc > 3 ? std::stof(argv[3]) : 0.5f;
        std::size_t iters = argc > 4 ? std::stoul(argv[4]) : 20;
        unsigned threads = argc > 5 ? std::stoul(argv[5]) : 0;
        return profile(n, theta, iters, threads);
    }
    return sweep();
}
