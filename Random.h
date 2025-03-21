#pragma once
#include <vector>
#include <random>

// Generate random numbers normally distributed
namespace Random
{
    inline std::random_device rd;  // Seed for random number engine
    inline std::mt19937 gen(rd()); // Mersenne Twister engine
    inline std::normal_distribution<std::float_t> normal_dist(0.0, 1.0); // Normal distribution

    inline std::float_t getNormalRandom()
    {
        return normal_dist(gen); // Generate a random number
    }

};
