#include "functions.h"
#include "Equity.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include </usr/local/Cellar/eigen/3.4.0_1/include/eigen3/Eigen/Dense>
#include <algorithm>  // For sorting
#include <stdexcept>  // For exception handling
#include <pybind11/embed.h>  // Pybind11 for embedding Python

namespace py = pybind11;

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

// import data for one ticker and return an Equity
Equity importOneTicker(const std::string& ticker_name, const std::uint16_t& share_no)
{
    // WARNING: using pybind11 to embed python works properly, but
    // it makes the code non-portable
    // use CSV files to load data instead
    py::scoped_interpreter guard{};  // Start Python interpreter
    py::module_ np = py::module_::import("numpy");
    py::module_ yf = py::module_::import("yfinance");

    const py::dict GLOBALS;  // Create a namespace to store the python variables
    GLOBALS["ticker"] = ticker_name;
    GLOBALS["share"] = share_no;

    // instantiate an equity with the default constructor
    Equity eq; // stack allocation

    py::exec(R"(
        import yfinance as yf
        import numpy as np
        stock = yf.download(ticker, start="2024-01-01", end="2025-01-01");
        print("Stock prices downloaded");

        # Calculate daily log returns and remove missing values
        stock["Returns"] = stock['Close'] / stock['Close'].shift(1);
        stock['Log Returns'] = np.log(stock['Close'] / stock['Close'].shift(1));
        stock = stock.dropna();

        # estimate mean and volatility
        mu = stock['Log Returns'].mean();
        sigma = stock['Log Returns'].std();

        # extract the Close price at the end of the time period
        all_S = stock["Close"].values.tolist()
        S0 = round(all_S[-1][0], 3)

        )", GLOBALS);

        const auto MU = GLOBALS["mu"].cast<std::float_t>();
        const auto SIGMA = GLOBALS["sigma"].cast<std::float_t>();
        const auto S0 = GLOBALS["S0"].cast<std::float_t>();
        const auto SHARES = GLOBALS["share"].cast<std::uint16_t>();
        const auto TICKER_NAME = GLOBALS["ticker"].cast<std::string>();

    // set member variables for the equity
    eq.setTicker(TICKER_NAME);
    eq.setShareNumber(SHARES);
    eq.setPrice(S0);
    eq.setMu(MU);
    eq.setSigma(SIGMA);

    return eq;
};

// Function to read Log Returns and Close Prices from a CSV file
//std::vector<double> readLogReturns(const std::string& filename)
std::pair<std::vector<double>, std::vector<double>> readLogReturns(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << filename << '\n';
        return {};
    }

    std::string line;
    std::string date;
    std::vector<double> logReturns;
    std::vector<double> closePrices;

    // Read the header and ignore it
    std::getline(file, line);

    while (getline(file, line))
    {
        std::stringstream ss(line);
        std::string value;
        double log_return;
        double close_price;

        // Read Date (ignore)
        std::getline(ss, date, ',');

        // Read Close
        std::getline(ss, value, ',');
        close_price = stod(value);

        // Read Returns (ignore)
        std::getline(ss, value, ',');

        // Read Log Returns
        std::getline(ss, value, ',');
        log_return = stod(value);

        closePrices.push_back(close_price);
        logReturns.push_back(log_return);
    }

    file.close();
    return {logReturns, closePrices};
}


// THIS FUNCTION IS NOT USED ANYMORE
// import data from yfinance for multiple tickers
//std::tuple<std::vector<std::float_t>, std::vector<std::vector<double>> > importMultipleTickers(const std::vector<std::string>& ticker_list, const std::vector<std::uint16_t>& share_no_list)
//std::tuple<std::vector<std::float_t>, std::vector<std::vector<double>>> importMultipleTickers(const std::vector<std::string>& ticker_list, const std::vector<std::uint16_t>& share_no_list)
std::float_t importMultipleTickers(std::vector<std::string>& ticker_list, const std::vector<std::uint16_t>& share_no_list)
{
    // WARNING: using pybind11 to embed python works properly, but
    // it makes the code non-portable
    // use CSV files to load data instead
    py::scoped_interpreter guard{};  // Start Python interpreter
    py::module_ np = py::module_::import("numpy");
    py::module_ yf = py::module_::import("yfinance");
    py::module_ sci = py::module_::import("scipy");

    const py::dict GLOBALS;  // Create a namespace to store the python variables

    // Convert C++ vector to Python list
    GLOBALS["tickers"] = py::cast(ticker_list);

    return 100.5f;  // std::make_tuple(LAST_PRICES, CHOLESKY_MATRIX);   // {LAST_PRICES, CHOLESKY_MATRIX};

}