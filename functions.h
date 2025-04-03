#pragma once
#include <vector>
#include "Equity.h"

// worst case out of the last simulation: it returns a scalar
std::float_t percentile(const std::vector<std::float_t>& data, std::float_t percent);

// worst case out of the last simulation: it returns an array, usually used to plot
std::vector<std::float_t> percentile_2D(const std::vector<std::vector<std::float_t>>& data, std::float_t percent);

// import data from yfinance for one ticker
Equity importOneTicker(const std::string& ticker_name, const std::uint16_t& share_no);

//std::vector<double> readLogReturns(const std::string& filename);
std::pair<std::vector<double>, std::vector<double>> readLogReturns(const std::string& filename);

// THIS FUNCTION IS NOT USED ANYMORE
// import data from yfinance for multiple tickers
//std::tuple<std::vector<std::float_t>, std::vector<std::vector<double>> > importMultipleTickers(const std::vector<std::string>& ticker_list, const std::vector<std::uint16_t>& share_no_list);
//std::tuple<std::vector<std::float_t>, std::vector<std::vector<double>>> importMultipleTickers(const std::vector<std::string>& ticker_list, const std::vector<std::uint16_t>& share_no_list);
std::float_t importMultipleTickers(const std::vector<std::string>& ticker_list, const std::vector<std::uint16_t>& share_no_list);