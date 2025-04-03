#include <string>
#include <iostream>
#include <utility>
#include "MultiEquityPortfolio.h"

#include <pybind11/detail/common.h>

MultiEquityPortfolio::MultiEquityPortfolio(Eigen::MatrixXd return_matrix, Eigen::VectorXd last_price_vector, const std::vector<std::uint16_t> &share_number_vector)
    : m_return_matrix{std::move(return_matrix)}
    , v_last_price_vector{std::move(last_price_vector)}
    , v_share_number_vector{share_number_vector}
{
    std::cout << "+++ MultiEquity Portfolio created" << "\n";
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
void MultiEquityPortfolio::setShareNumberVector(const std::vector<std::uint16_t> &share_number_vector)
{
    v_share_number_vector = share_number_vector;
}


