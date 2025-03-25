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

namespace Global
{
    namespace py = pybind11;
    // number of montecarlo simulations to run: a low number of simulations (<= 10) is not advisable
    constexpr std::int16_t SIMULATIONS { 1000 };
    // Ito's lemma: used in the Geometric Brownian Motion and the Stochastic Calculus
    constexpr std::float_t ITO { 0.5 };
    // SHARES is the number of shares, S0 is the last price: error if constexpr is used
    // level of confidence
    constexpr std::float_t CONF_LEVEL { 5.0 };
    // daily time step: if weekly then 1/52
    constexpr std::float_t DT { 1.0f / 252.0f };
}


int main() {
    Portfolio dumbPortfolio;

    // the tickers can be inputted at run-time
    // std::string is used because std::string_view can cause dangling references in the Portfolio
    const std::vector<std::string> TICKERS = { "AAPL"};

    const std::vector<std::int16_t> TICKERS_SHARES {10};

    if (TICKERS.size() == 1)
    {
        dumbPortfolio.addEquity(importOneTicker(TICKERS[0], TICKERS_SHARES[0]));
    }

    // print all the equities in the portfolio
    for (const auto& equity : dumbPortfolio.getEquities()) {
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
        // variables to use for random prices
        const std::float_t DRIFT = (mu - Global::ITO * std::pow(sigma, 2.0f)) * Global::DT;
        const std::float_t DIFFUSION_COEFF = sigma * std::sqrt(Global::DT);

        // initialize an empty 2-D vector
        std::vector<std::vector<std::float_t>> simulated_prices;

        if (STEPS == 1)
        {
            // matrix size is STEPS + 1 x SIMULATIONS (rows x columns)
            // assign the portfolio value to the first row [0]: it will be the starting point
            simulated_prices = std::vector(STEPS + 1, std::vector(Global::SIMULATIONS, 0.0f));
            for (int i = 0; i < Global::SIMULATIONS; ++i)
            {
                simulated_prices[0][i] = dumbPortfolio.getPortfolioValue();
            }

            for (int i = 0; i < Global::SIMULATIONS; ++i)
            {
                // row [1] will have the prices randomly generated below
                const std::float_t rand = Random::getNormalRandom();
                simulated_prices[1][i] = simulated_prices[0][i] * std::exp(DRIFT + DIFFUSION_COEFF * rand);
            }
        }
        else if (STEPS > 1 && STEPS <= 252)
        {
            // matrix size is STEPS x SIMULATIONS (rows x columns)
            // assign the portfolio value to the first row [0]: it will be the starting point
            simulated_prices = std::vector(STEPS, std::vector(Global::SIMULATIONS, 0.0f));
            for (int i = 0; i < Global::SIMULATIONS; ++i)
            {
                simulated_prices[0][i] = dumbPortfolio.getPortfolioValue();
            }

            // Generate normally distributed random prices for rows >= [1]
            for (int t = 1; t < STEPS; ++t)
            {
                for (int i = 0; i < Global::SIMULATIONS; ++i)
                {
                    const std::float_t rand = Random::getNormalRandom();
                    simulated_prices[t][i] = simulated_prices[t - 1][i] * std::exp(DRIFT + DIFFUSION_COEFF * rand);
                }
            }
        }

    // last row of the matrix
    std::vector<std::float_t> last_simulation_prices = simulated_prices.back();

    // Calculate the percentile
    const std::float_t percentile_95 = percentile(last_simulation_prices, Global::CONF_LEVEL);
    std::cout << "Lowest value of the portfolio (95% confidence level): " << percentile_95 << std::endl;

    // this can be useful to plot
    std::vector<std::float_t> percentile_95_array = percentile_2D(simulated_prices, Global::CONF_LEVEL);

    // calculate the VaR and the VaR %
    const std::float_t VaR_95 { percentile_95 - dumbPortfolio.getPortfolioValue() };
    const std::float_t max_loss_95 { (dumbPortfolio.getPortfolioValue() - percentile_95) / dumbPortfolio.getPortfolioValue() * 100.0f };

    std::cout << "VaR in $ at 95% after " << TRADING_DAYS << " days: " << std::setprecision(3) << VaR_95 << '\n';
    std::cout << "VaR % at 95% after: " << TRADING_DAYS << " days: -" << std::setprecision(3) << max_loss_95 << "%" << '\n';

    }
    else if (dumbPortfolio.getItemCount() > 1)
    {

    }

    return 0;
};