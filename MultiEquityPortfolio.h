#pragma once
#include <string>
#include <vector>
#include </usr/local/Cellar/eigen/3.4.0_1/include/eigen3/Eigen/Dense>
#include </usr/local/Cellar/eigen/3.4.0_1/include/eigen3/unsupported/Eigen/CXX11/Tensor>

class MultiEquityPortfolio
{
private:
    Eigen::MatrixXd m_return_matrix;
    Eigen::VectorXd v_last_price_vector;
    std::vector<std::string> v_tickers_vector;
    std::vector<std::uint16_t> v_share_number_vector;
public:
    MultiEquityPortfolio() = default;

    MultiEquityPortfolio(Eigen::MatrixXd return_matrix, Eigen::VectorXd last_price_vector, const std::vector<std::string> &tickers_vector, const std::vector<std::uint16_t> &share_number_vector);

    // getters
    Eigen::MatrixXd getReturnMatrix() const;
    Eigen::VectorXd getLastPriceVector() const;
    std::vector<std::string> getTickers() const;
    std::vector<std::uint16_t> getShareNumberVector() const;

    // setters
    void setReturnMatrix(const Eigen::MatrixXd& return_matrix);
    void setLastPriceVector(const Eigen::VectorXd& last_price_vector);
    void setTickersVector(const std::vector<std::string> &tickers_vector);
    void setShareNumberVector(const std::vector<std::uint16_t> &share_number_vector);

    // Add begin() and end() to support range-based loops
    std::vector<std::string>::const_iterator begin() const;
    std::vector<std::string>::const_iterator end() const;

    // Return a vector with the mean values
    Eigen::VectorXd getMean() const;

    // Return annualized covariance matrix
    Eigen::MatrixXd  getReturnCovarianceMatrix() const;

};
