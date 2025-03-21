#include "functions.h"
#include <iostream>
#include <vector>
#include <algorithm>  // For sorting
#include <stdexcept>  // For exception handling

// worst case out of the last simulation with a level of confidence: it returns a scalar
std::float_t percentile(const std::vector<std::float_t>& data, const std::float_t percent)
{
    if (data.empty())
    {
        throw std::runtime_error("Data vector is empty.");
    }

    std::vector<std::float_t> sorted_data = data;  // Copy data to avoid modifying the original
    std::sort(sorted_data.begin(), sorted_data.end());  // Sort the data

    // Compute the index corresponding to the percentile
    std::float_t index = (percent / 100.0f) * (sorted_data.size() - 1);
    const std::uint16_t lower_idx = static_cast<int>(index);  // Integer part
    const std::float_t fraction = index - lower_idx;      // Fractional part

    // Linear interpolation for better accuracy
    if (lower_idx + 1 < sorted_data.size())
    {
        return sorted_data[lower_idx] + fraction * (sorted_data[lower_idx + 1] - sorted_data[lower_idx]);
    } else {
        return sorted_data[lower_idx];  // If at the last index, just return the value
    }
};

std::vector<float> percentile_2D(const std::vector<std::vector<float>>& data, float percent)
{
    std::vector<float> result;

    for (const auto& row : data)
    {
        if (!row.empty())
        {
            result.push_back(percentile(row, percent));
        } else {
            result.push_back(0.0f); // Handle empty row
        }
    }

    return result;
};
