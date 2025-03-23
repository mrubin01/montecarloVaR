#include <pybind11/embed.h>  // Pybind11 for embedding Python
#include <iostream>
#include <limits>
#include <vector>
#include <cmath>
#include <iomanip>
#include <cstdlib> // Required for exit()
#include "Equity.h"
#include "functions.h"
#include "Random.h"
#include "Portfolio.h"

namespace py = pybind11;

int main() {
    Portfolio dumbPortfolio;
    //dumbPortfolio.getPortfolioValue();

    // the tickers can be inputted at run-time
    // std::string is used because std::string_view can cause dangling references in the Portfolio
    const std::vector<std::string> TICKERS = { "AAPL"};
    // number of montecarlo simulations to run
    constexpr std::int16_t SIMULATIONS { 1000 };
    // level of confidence
    constexpr std::float_t CONF_LEVEL { 5.0 };
    // Ito's lemma: used in the Geometric Brownian Motion and the Stochastic Calculus
    constexpr std::float_t ITO { 0.5 };
    // SHARES is the number of shares, S0 is the last price: error if constexpr is used
    const std::vector<std::int16_t> TICKERS_SHARES {10, 20, 30};
    // daily time step: if weekly then 1/52
    constexpr std::float_t DT { 1.0f / 252.0f };

    // WARNING: using pybind11 to embed python works properly, but
    // it makes the code non-portable
    // use CSV files to load data instead
    py::scoped_interpreter guard{};  // Start Python interpreter
    py::module_ np = py::module_::import("numpy");
    py::module_ yf = py::module_::import("yfinance");

    for (size_t i = 0; i < TICKERS.size(); i++)
    {
        const py::dict GLOBALS;  // Create a namespace to store the python variables
        GLOBALS["ticker"] = TICKERS[i];
        GLOBALS["share"] = TICKERS_SHARES[i];

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

        const Equity e{TICKER_NAME, SHARES, S0, MU, SIGMA};
        dumbPortfolio.addEquity(e);

    }

    // print all the equities in the portfolio
    for (const auto& equity : dumbPortfolio.getEquities())
    {
        std::cout << "Equity " << equity.getTicker() << " - MU " << equity.getMu() << " - SIGMA " << equity.getSigma() << "\n";
    }

    std::cout << "Portfolio size: " << dumbPortfolio.getItemCount() << '\n';
    std::cout << "Initial Portfolio value: " << dumbPortfolio.getPortfolioValue() << '\n';


    // the portfolio contains only one item: the Cholesky decomposition is not necessary
    if (dumbPortfolio.getItemCount() == 1)
    {
        std::float_t mu {};
        std::float_t sigma {};
        for (const auto& equity : dumbPortfolio.getEquities())
        {
            mu = equity.getMu();
            sigma = equity.getSigma();
        }

        // input trading days and validate the data type: >= 1 and <= 252)
        std::int16_t TRADING_DAYS{};
        std::cout << "How many trading days (must be >= 1 and <= 252): " << '\n';
        while (!(std::cin >> TRADING_DAYS) || TRADING_DAYS > 252)
        {
            std::cin.clear();  // Clear error flag
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // Discard invalid input
            std::cout << "Invalid input. Please enter an integer between 1 and 252: ";
        }

        // number of steps
        const int16_t STEPS = TRADING_DAYS;

        // create an empty matrix STEPS (+ 1) x SIMULATIONS (rows x columns) and
        // assign the portfolio value to the first row: it will be the starting point
        std::vector<std::vector<std::float_t>> simulated_prices;
        if (STEPS == 1)
        {
            simulated_prices = std::vector(STEPS + 1, std::vector(SIMULATIONS + 1, 0.0f));
            for (int i = 0; i < SIMULATIONS; ++i)
            {
                simulated_prices[0][i] = dumbPortfolio.getPortfolioValue();
            }
        }
        else if (STEPS > 1 && STEPS <= 252)
        {
            simulated_prices = std::vector(STEPS, std::vector(SIMULATIONS + 1, 0.0f));
            for (int i = 0; i < SIMULATIONS; ++i)
            {
                simulated_prices[0][i] = dumbPortfolio.getPortfolioValue();
            }
        };

        // if STEPS=21 and SIMULATIONS=100, it will be a matrix 21x100
        // each column is a simulation starting from the initial value S0
        const std::float_t DRIFT = (mu - ITO * std::pow(sigma, 2.0f)) * DT;
        const std::float_t DIFFUSION_COEFF = sigma * std::sqrt(DT);

        // Iterate over time steps
        for (int t = 1; t < STEPS; ++t) {
            for (int i = 0; i < SIMULATIONS; ++i) {
                // Generate normally distributed random number
                const std::float_t rand = Random::getNormalRandom();
                simulated_prices[t][i] = simulated_prices[t - 1][i] * std::exp(DRIFT + DIFFUSION_COEFF * rand);
            }
        }

        // last row of the matrix
        std::vector<std::float_t> last_simulation_prices = simulated_prices.back();

        // Calculate the percentile
        const std::float_t percentile_95 = percentile(last_simulation_prices, CONF_LEVEL);
        std::cout << "5th percentile (95% confidence level): " << percentile_95 << std::endl;

        // this can be useful to plot
        std::vector<std::float_t> percentile_95_array = percentile_2D(simulated_prices, CONF_LEVEL);

        // calculate the VaR and the VaR %
        const std::float_t VaR_95 { percentile_95 - dumbPortfolio.getPortfolioValue() };
        const std::float_t max_loss_95 { (dumbPortfolio.getPortfolioValue() - percentile_95) / dumbPortfolio.getPortfolioValue() * 100.0f };

        std::cout << "VaR in $ at 95% after " << TRADING_DAYS << " days: " << std::setprecision(3) << VaR_95 << '\n';
        std::cout << "VaR % at 95% after: " << TRADING_DAYS << " days: -" << std::setprecision(3) << max_loss_95 << "%" << '\n';

    }

    return 0;
};