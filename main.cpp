#include <pybind11/embed.h>  // Pybind11 for embedding Python
#include <iostream>
#include <limits>
#include <vector>
#include <cmath>
#include <iomanip>
#include "Equity.h"
#include "functions.h"
#include "Random.h"
#include "Portfolio.h"

namespace py = pybind11;

int main() {

    const std::vector<std::string_view> PORTFOLIO_TICKERS = { "AAPL", "MSFT", "TSLA"};
    // number of montecarlo simulations to run
    constexpr std::int16_t SIMULATIONS { 1000 };
    // level of confidence
    constexpr std::float_t CONF_LEVEL { 5.0 };
    // Ito's lemma: used in the Geometric Brownian Motion and the Stochastic Calculus
    constexpr std::float_t ITO { 0.5 };

    // WARNING: using pybind11 to embed python works properly, but
    // it makes the code non-portable
    // use CSV files to load data instead
    py::scoped_interpreter guard{};  // Start Python interpreter
    const py::dict GLOBALS;  // Create a namespace to store the python variables
    py::module_ np = py::module_::import("numpy");
    py::module_ yf = py::module_::import("yfinance");

    py::exec(R"(
        import yfinance as yf
        import numpy as np
        stock = yf.download("AAPL", start="2024-01-01", end="2025-01-01");
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

    // SHARES is the number of shares, S0 is the last price
    constexpr std::int16_t SHARES { 10 };
    const auto S0 = GLOBALS["S0"].cast<std::float_t>();
    const auto MU = GLOBALS["mu"].cast<std::float_t>();
    const auto SIGMA = GLOBALS["sigma"].cast<std::float_t>();

    const Equity e1{"AAPL", SHARES, S0};
    const Equity e2{"MSFT", SHARES, 120.0f};

    Portfolio dumbPortfolio = {e1, e2};
    dumbPortfolio.getPortfolioValue();

    std::cout << "Equity " << e1.getTicker() << " last price, mean and std: " << S0 << ", " << MU << ", " << SIGMA << '\n';
    std::cout << "Overall value of the shares: " << e1.getSharesValue() << '\n';

    // daily time step: if weekly then 1/52
    constexpr std::float_t DT { 1.0f / 252.0f };

    // input trading days and validate the data type: >= 1 and <= 252)
    std::int16_t TRADING_DAYS{ 21 }; // the code below must be uncommented
    // std::cout << "How many trading days (must be >= 1 and <= 252): " << '\n';
    // while (!(std::cin >> TRADING_DAYS) || TRADING_DAYS > 252)
    // {
    //     std::cin.clear();  // Clear error flag
    //     std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // Discard invalid input
    //     std::cout << "Invalid input. Please enter an integer between 1 and 252: ";
    // }

    // number of steps
    const int16_t STEPS = TRADING_DAYS;

    // create an empty matrix STEPS x SIMULATIONS (rows x columns) and
    // assign the last Close price * shares to the first row: it will be the starting point
    std::vector<std::vector<std::float_t>> simulated_prices;
    if (TRADING_DAYS == 1)
    {
        simulated_prices = std::vector(STEPS + 1, std::vector(SIMULATIONS + 1, 0.0f));
        for (int i = 0; i < SIMULATIONS; ++i)
        {
            simulated_prices[0][i] = e1.getSharesValue();
        }
    }
    else if (TRADING_DAYS > 1 || TRADING_DAYS <= 252)
    {
        simulated_prices = std::vector(STEPS, std::vector(SIMULATIONS + 1, 0.0f));
        for (int i = 0; i < SIMULATIONS; ++i)
        {
            simulated_prices[0][i] = e1.getSharesValue();
        }
    };

    std::cout << '\n' << "Initial portfolio value = $" << simulated_prices[0][0] << '\n';

    // if SIMULATIONS=100 and STEPS=21, it will be a matrix 21x100
    // each column is a simulation starting from the initial value S0
    const std::float_t DRIFT = (MU - ITO * std::pow(SIGMA, 2.0f)) * DT;
    const std::float_t DIFFUSION_COEFF = SIGMA * std::sqrt(DT);

    // Iterate over time steps
    for (int t = 1; t < STEPS; ++t) {
        for (int i = 0; i < SIMULATIONS; ++i) {
            //std::float_t rand = normal_dist(gen);  // Generate normally distributed random number
            const std::float_t rand = Random::getNormalRandom();
            simulated_prices[t][i] = simulated_prices[t - 1][i] * std::exp(DRIFT + DIFFUSION_COEFF * rand);
        }
    }
    // test the last row has correct values
    // std::cout << "First 10 values of last row: ";
    // for (int i = 0; i < 10; ++i) {
    //     std::cout << simulated_prices[STEPS - 1][i] << " ";
    // }
    // std::cout << std::endl;

    // last row of the matrix
    std::vector<std::float_t> last_day_prices = simulated_prices.back();

    // Calculate the 5th percentile
    const std::float_t percentile_95 = percentile(last_day_prices, CONF_LEVEL);

    std::cout << "5th percentile (95% confidence level): " << percentile_95 << std::endl;

    // this can be useful to plot
    std::vector<std::float_t> percentile_95_array = percentile_2D(simulated_prices, CONF_LEVEL);

    const std::float_t VaR_95 { percentile_95 - e1.getSharesValue() };
    const std::float_t max_loss_95 { (e1.getSharesValue() - percentile_95) / e1.getSharesValue() * 100.0f };

    std::cout << "VaR in $ at 95% after " << TRADING_DAYS << " days: " << std::setprecision(3) << VaR_95 << '\n';
    std::cout << "VaR % at 95% after: " << TRADING_DAYS << " days: -" << std::setprecision(3) << max_loss_95 << "%" << '\n';

    const Equity e3{"TSLA", SHARES, 150.0f};
    dumbPortfolio.addEquity(e3);

    dumbPortfolio.removeEquity("AAPL");

    dumbPortfolio.getPortfolioValue();

    return 0;
};