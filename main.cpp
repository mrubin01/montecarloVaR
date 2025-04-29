#include <pybind11/embed.h>  // Pybind11 for embedding Python
#include <pybind11/stl.h>  // Required for automatic conversion of STL containers
#include <iostream>
#include <limits>
#include <vector>
#include <cmath>
#include <iomanip>
#include <random>
#include <cmath>
#include <cstdlib> // Required for exit()
#include </usr/local/Cellar/eigen/3.4.0_1/include/eigen3/Eigen/Dense>
#include </usr/local/Cellar/eigen/3.4.0_1/include/eigen3/unsupported/Eigen/CXX11/Tensor>
#include "Equity.h"
#include "functions.h"
#include "MultiEquityPortfolio.h"
#include "Random.h"
#include "Portfolio.h"

namespace Global
{
    // number of montecarlo simulations to run: a low number of simulations (<= 10) is not advisable
    constexpr std::int16_t SIMULATIONS { 1000 };
    // days to forecast
    constexpr std::int16_t TRADING_DAYS { 5 };
    // Ito's lemma: used in the Geometric Brownian Motion and the Stochastic Calculus
    constexpr std::float_t ITO { 0.5 };
    // SHARES is the number of shares, S0 is the last price: error if constexpr is used
    // level of confidence
    constexpr std::float_t CONF_LEVEL { 5.0 };
    // daily time step: if weekly then 1/52
    constexpr std::float_t DT { 1.0f / 252.0f };

    // tickers and number of shares can be inputted at run-time?
    // std::string is used because std::string_view can cause dangling references in the Portfolio
    const std::vector<std::string> TICKERS = {"AAPL", "CIM", "CVX"};
    const std::vector<std::uint16_t> TICKERS_SHARES = {10, 15, 20};

    const std::vector<std::string> PATH_LIST = {"AAPL.csv", "CIM.csv", "CVX.csv"};

}

int main()
{

    if (Global::TICKERS.size() == 1)
    {
        std::cout << "Loading one ticker..." << '\n';

        Portfolio dumbPortfolio; // stack allocation
        dumbPortfolio.addEquity(importOneTicker(Global::TICKERS[0], Global::TICKERS_SHARES[0]));

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
    }

    else if (Global::TICKERS.size() > 1)
    {
        std::cout << "Loading multiple tickers..." << "\n\n";

        // Extract the number of days only for the first ticker: it will be used to check the others'
        auto [apple_returns, apple_prices] = readLogReturns(Global::PATH_LIST[0]);
        size_t n_days = apple_returns.size(); // this is the number of days in the csv file


        // The matrix will store the log returns, the vector the last known prices
        Eigen::MatrixXd logReturnsMatrix(n_days, Global::TICKERS.size());
        Eigen::VectorXd last_prices(Global::TICKERS.size());

        // In the Eigen matrix logReturnsMatrix each column is the stock log returns, the rows are n_days
        for (int col = 0; col < Global::TICKERS.size(); col++)
        {
            auto [returns, prices] = readLogReturns(Global::PATH_LIST[col]);

            // Ensure that vectors have all the same size
            if (returns.size() != n_days)
            {
                std::cerr << "Error: CSV files have different numbers of days!" << '\n';
                return 1;
            }

            for (int row = 0; row < n_days; row++)
            {
                logReturnsMatrix(row, col) = returns[row];
            }

            double last = prices.back();
            last_prices(col) = last;
        }

        MultiEquityPortfolio newPortfolio(logReturnsMatrix, last_prices, Global::TICKERS, Global::TICKERS_SHARES);

        // a vector with the mean values and a covariance matrix
        const Eigen::VectorXd meanVector = newPortfolio.getMean();
        const Eigen::MatrixXd annualizedCovarianceMatrix = newPortfolio.getReturnCovarianceMatrix();

        //std::cout << "Mean of each stock:\n" << meanVector << '\n';
        //std::cout << "Covariance Matrix:\n" << annualizedCovarianceMatrix << "\n\n";

        // Compute Cholesky decomposition
        Eigen::LLT<Eigen::MatrixXd> llt(annualizedCovarianceMatrix);
        if (llt.info() == Eigen::NumericalIssue)
        {
            std::cerr << "Error: Covariance matrix is not positive definite!" << std::endl;
            return 1;
        }

        // Lower triangular matrix of the Cholesky decomposition matrixx
        Eigen::MatrixXd L = llt.matrixL();

        // Create a random number generator with normal distribution
        std::random_device rd;
        std::mt19937 gen(rd());
        std::normal_distribution<> d(0.0, 1.0);

        // Create a 3D tensor (TRADING_DAYS, num_tickers, SIMULATIONS) to hold the random values
        Eigen::Tensor<double, 3> rand_normals(Global::TRADING_DAYS, Global::TICKERS.size(), Global::SIMULATIONS);

        // This code is used to avoid that random values are not zero
        for (int i = 0; i < Global::TRADING_DAYS; i++) {
            for (int j = 0; j < Global::TICKERS.size(); j++) {
                for (int k = 0; k < Global::SIMULATIONS; k++) {
                    double value;
                    do {
                        value = d(gen);  // Generate random value
                    } while (value == 0.0); // Ensure it's not zero
                    rand_normals(i, j, k) = value;
                }
            }
        }

        // Example: Print a specific value (e.g., row 0, ticker 1, simulation 0)
        //std::cout << "First random value (row 0, ticker 1, sim 0): " << rand_normals(0, 1, 10) << std::endl;

        // Correlated shocks tensor (TRADING_DAYS, TICKERS.size(), SIMULATIONS)
        // shocks in simulations are unexpected changes in the asset prices
        Eigen::Tensor<double, 3> correlated_shocks(Global::TRADING_DAYS, Global::TICKERS.size(), Global::SIMULATIONS);

        // Define 2 empty matrices
        Eigen::MatrixXd rand_matrix;
        Eigen::MatrixXd result_matrix;

        //Perform matrix multiplication: correlated_shocks = L * rand_normals
        for (int t = 0; t < Global::TRADING_DAYS; t++)
        {
            rand_matrix.resize(Global::TICKERS.size(), Global::SIMULATIONS);
            result_matrix.resize(Global::TICKERS.size(), Global::SIMULATIONS);

            // Extract (TICKERS.size() x Global::SIMULATIONS) slice from rand_normals
            for (int j = 0; j < Global::TICKERS.size(); j++)
            {
                for (int s = 0; s < Global::SIMULATIONS; s++)
                {
                    rand_matrix(j, s) = rand_normals(t, j, s);
                }
            }

            // Perform matrix multiplication: result_matrix = L * rand_matrix
            result_matrix = L * rand_matrix;

            // Store the result back in the correlated_shocks tensor
            for (int i = 0; i < Global::TICKERS.size(); i++)
            {
                for (int s = 0; s < Global::SIMULATIONS; s++)
                {
                    correlated_shocks(t, i, s) = result_matrix(i, s);
                }
            }
        }

        // Example: Print first correlated shock value
        //std::cout << "First correlated shock 1: " << correlated_shocks(0, 0, 0) << std::endl;
        //std::cout << "First correlated shock 2: " << correlated_shocks(20, 2, 999) << std::endl;
        //std::cout << correlated_shocks.dimensions() << '\n';

        // This code is used only to check zeros in the tensors, then it can be deleted
        // std::uint16_t zeros;
        // for (int i = 0; i < 21; i++)
        // {
        //     for (int j = 0; j < 3; j++)
        //     {
        //         for (int k = 0; k < 1000; k++)
        //         {
        //             if (correlated_shocks(i, j, k) == 0.0f)
        //             {
        //                 zeros += 1;
        //             }
        //         }
        //     }
        // }
        // std::cout << zeros << '\n';

        // SO FAR rand_normals AND correlated_shocks HAVE NO ZERO VALUES AND BOTH SHAPE [TRADING_DAYS, TICKERS, SIMULATIONS]

        // Simulate price paths using GBM
        // Initialize a 3D tensor and set the prices to zero (22 x 3 x 1000)
        Eigen::Tensor<double, 3> simulated_prices(Global::TRADING_DAYS + 1, Global::TICKERS.size(), Global::SIMULATIONS);
        simulated_prices.setZero();


        // Assign the last known price for each equity to simulated_prices[0, :, :]
        for (int j = 0; j < Global::TICKERS.size(); ++j)
        {
            for (int s = 0; s < Global::SIMULATIONS; ++s) {
                simulated_prices(0, j, s) = last_prices(j);
            }
        }


        // These constant variables will be used to calculate the simulated prices
        const Eigen::VectorXd covDiagonal = annualizedCovarianceMatrix.diagonal();
        const Eigen::VectorXd drift = (meanVector.array() - Global::ITO * covDiagonal.array()) * Global::DT;
        double sqrtDt = sqrt(Global::DT);

        for (int t = 1; t < Global::TRADING_DAYS + 1; t++)
        {
             // Iterate over assets and simulations
             for (int j = 0; j < Global::TICKERS.size(); ++j) {
                 for (int s = 0; s < Global::SIMULATIONS; ++s) {
                     // Compute diffusion: correlated_shocks[t - 1, j, s] * sqrt(dt)
                     double diffusion = correlated_shocks(t - 1, j, s) * sqrtDt;

                     // Compute price update using GBM: S_t = S_t-1 * exp(drift + diffusion)
                     simulated_prices(t, j, s) = simulated_prices(t - 1, j, s) * std::exp(drift(j) + diffusion);
                 }
             }
        }

        // Print a sample value for verification: index1 is the trading days, index2 is the equity, index3 is the simulation
        // std::cout << "Simulated price at T=21, Asset=0, Simulation=0: " << simulated_prices(21, 1, 0) << std::endl;

        // This is value of the portfolio before the simulations
        double portfolio_initial_value {last_prices[0] * Global::TICKERS_SHARES[0]
            + last_prices[1] * Global::TICKERS_SHARES[1] + last_prices[2] * Global::TICKERS_SHARES[2]};

        std::cout << '\n' << "Portfolio value before the simulations: $" << portfolio_initial_value << "\n\n";

        // Extract the last simulation
        Eigen::MatrixXd last_simulation(Global::TICKERS.size(), Global::SIMULATIONS);
        for (int i = 0; i < Global::TICKERS.size(); i++)
        {
            for (int j = 0; j < Global::SIMULATIONS; ++j)
            {
                last_simulation(i, j) = simulated_prices(Global::TRADING_DAYS, i, j);
            }
        }


        // Convert the vector TICKERS_SHARES into an Eigen vector of doubles
        Eigen::VectorXd number_of_shares(3);
        for (size_t i = 0; i < Global::TICKERS_SHARES.size(); i++)
        {
            number_of_shares(i) = static_cast<double>(Global::TICKERS_SHARES[i]);
        }

        Eigen::MatrixXd last_simulation_transposed = last_simulation.transpose(); // (1000x3)
        // Perform a multiplication matrix-vector: (1000x3) * (3x1) = (1000x1)
        Eigen::VectorXd final_values = last_simulation_transposed * number_of_shares;


        // Print part of the result
        // std::cout << "Result (first 5 elements):\n" << final_values.head(5) << "\n...\n";

        // Compute profit/loss distribution
        Eigen::VectorXd losses = (Eigen::VectorXd::Constant(final_values.size(), portfolio_initial_value) - final_values).matrix();


        // Convert Eigen::VectorXd to std::vector for sorting
        std::vector<double> values(losses.data(), losses.data() + losses.size());

        // Sort the values (ascending order)
        std::sort(values.begin(), values.end());

        // Compute the index for the 5th percentile (VaR_95)
        auto index95 = static_cast<size_t>(std::round(0.05 * (values.size() - 1)));  // 5th percentile index

        // Extract the 5th percentile value
        double VaR_95 = values[index95];
        double VaR_95_perc {VaR_95 / portfolio_initial_value * 100.0f};

        // Calculate the Expected Shortfall beyond the confidence level 95%
        double es_sum_95 { 0.0 };
        double indexes_es_95 { 0.0 };
        for (size_t i = 0; i < (index95 - 1); i++) {
            indexes_es_95 += 1;
            es_sum_95 += values[i];  // sum values beyond index95
        }
        double es_mean_95 = es_sum_95 / indexes_es_95;

        std::cout << "Value at Risk after " << Global::TRADING_DAYS <<  " days (confidence level 95%): " << VaR_95 << '\n';
        std::cout << "Value at Risk % : " << std::setprecision(3) << VaR_95_perc << '\n';
        std::cout << std::setprecision(6) << std::defaultfloat; // this resets the precision for the following value
        std::cout << "Expected Shortfall (ES) beyond 95% : " << es_mean_95 << std::endl;

        // Compute the index for the 1st percentile
        auto index99 = static_cast<size_t>(std::round(0.01 * (values.size() - 1)));  // 1st percentile index

        // Extract the 1st percentile value
        double VaR_99 = values[index99];
        double VaR_99_perc {VaR_99 / portfolio_initial_value * 100.0f};

        // Calculate the Expected Shortfall beyond the confidence level 99%
        double es_sum_99 { 0.0 };
        double indexes_es_99 { 0.0 };
        for (size_t i = 0; i < (index99 - 1); i++) {
            indexes_es_99 += 1;
            es_sum_99 += values[i];  // sum values beyond index95
        }
        double es_mean_99 = es_sum_99 / indexes_es_99;

        std::cout << "\n=======================================\n" << std::endl;
        std::cout << "Value at Risk after " << Global::TRADING_DAYS <<  " days (confidence level 99%): " << VaR_99 << std::endl;
        std::cout << "Value at Risk % : " << std::setprecision(3) << VaR_99_perc << std::endl;
        std::cout << std::setprecision(6) << std::defaultfloat; // this resets the precision for the following value
        std::cout << "Expected Shortfall (ES) beyond 99% : " << es_mean_99 << std::endl;


    }

    return 0;
}