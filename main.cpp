#include <pybind11/embed.h>  // Pybind11 for embedding Python
#include <iostream>
#include "Equity.h"

namespace py = pybind11;

int main() {
    py::scoped_interpreter guard{};  // Start Python interpreter
    py::dict globals;  // Create a namespace to store the python variables
    py::module_ np = py::module_::import("numpy");
    py::module_ yf = py::module_::import("yfinance");

    py::exec(R"(
        import yfinance as yf
        import numpy as np
        stock = yf.download("GOOGL", start="2024-01-01", end="2025-01-01");
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

    )", globals);

    const auto S0 = globals["S0"].cast<float_t>();
    const auto mu = globals["mu"].cast<float_t>();
    const auto sigma = globals["sigma"].cast<float_t>();

    Equity e1{"GOOG", 10, 1500, 150};

    std::cout << "Equity " << e1.getTicker() << " last price, mean and std: " << S0 << ", " << mu << ", " << sigma << '\n';

    return 0;
};