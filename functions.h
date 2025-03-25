#pragma once
#include <vector>
#include "Equity.h"

// worst case out of the last simulation: it returns a scalar
std::float_t percentile(const std::vector<std::float_t>& data, std::float_t percent);

// worst case out of the last simulation: it returns an array, usually used to plot
std::vector<std::float_t> percentile_2D(const std::vector<std::vector<std::float_t>>& data, std::float_t percent);

// import data from yfinance
Equity importOneTicker(const std::string& ticker_name, const std::uint16_t& share_no);

