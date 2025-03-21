#pragma once
#include <vector>

// worst case out of the last simulation: it returns a scalar
std::float_t percentile(const std::vector<std::float_t>& data, std::float_t percent);

// worst case out of the last simulation: it returns an array, usually used to plot
std::vector<std::float_t> percentile_2D(const std::vector<std::vector<std::float_t>>& data, std::float_t percent);
