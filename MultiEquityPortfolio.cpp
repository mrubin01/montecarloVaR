#include <string>
#include <iostream>
#include <utility>
#include "MultiEquityPortfolio.h"

#include <pybind11/detail/common.h>

MultiEquityPortfolio::MultiEquityPortfolio(Eigen::MatrixXd return_matrix, Eigen::VectorXd last_price_vector, const std::vector<std::string> &tickers_vector, const std::vector<std::uint16_t> &share_number_vector)
    : m_return_matrix{std::move(return_matrix)}
    , v_last_price_vector{std::move(last_price_vector)}
    , v_tickers_vector{tickers_vector}
    , v_share_number_vector{share_number_vector}
{
    std::cout << "+++ MultiEquity Portfolio created +++" << "\n";
}

// getters
Eigen::MatrixXd MultiEquityPortfolio::getReturnMatrix() const
{
    return m_return_matrix;
}
Eigen::VectorXd MultiEquityPortfolio::getLastPriceVector() const
{
    return v_last_price_vector;
}
std::vector<std::string> MultiEquityPortfolio::getTickers() const
{
    return v_tickers_vector;
}
std::vector<std::uint16_t> MultiEquityPortfolio::getShareNumberVector() const
{
    return v_share_number_vector;
}

// setters
void MultiEquityPortfolio::setReturnMatrix(const Eigen::MatrixXd &return_matrix)
{
    m_return_matrix = return_matrix;
}
void MultiEquityPortfolio::setLastPriceVector(const Eigen::VectorXd &last_price_vector)
{
    v_last_price_vector = last_price_vector;
}
void MultiEquityPortfolio::setTickersVector(const std::vector<std::string> &tickers_vector)
{
    v_tickers_vector = tickers_vector;
}
void MultiEquityPortfolio::setShareNumberVector(const std::vector<std::uint16_t> &share_number_vector)
{
    v_share_number_vector = share_number_vector;
}

// Add begin() and end() to support range-based loops
std::vector<std::string>::const_iterator MultiEquityPortfolio::begin() const
{
    return v_tickers_vector.begin();
};
std::vector<std::string>::const_iterator MultiEquityPortfolio::end() const
{
    return v_tickers_vector.end();
};

Eigen::VectorXd MultiEquityPortfolio::getMean() const
{
    const Eigen::VectorXd mean_vector = m_return_matrix.colwise().mean();

    return mean_vector;
}

// From the log return matrix extract a vector with the mean values and calculate the annualized Covariance Matrix
// do not multiply the mean * 252, otherwise the numbers will be wrong
Eigen::MatrixXd MultiEquityPortfolio::getReturnCovarianceMatrix() const
{
    //Eigen::VectorXd mean_vector = m_return_matrix.colwise().mean();
    Eigen::VectorXd mean_vector = getMean();
    Eigen::RowVectorXd mean_row = mean_vector.transpose();

    const Eigen::MatrixXd centered = m_return_matrix.rowwise() - mean_row;
    const Eigen::MatrixXd covarianceMatrix = (centered.adjoint() * centered) / static_cast<double>(m_return_matrix.rows() - 1);
    const Eigen::MatrixXd annualizedCovarianceMatrix = covarianceMatrix * 252;

    return annualizedCovarianceMatrix;
}