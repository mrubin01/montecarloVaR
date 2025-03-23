#include "Equity.h"
#include <string>
#include <iostream>
#include <utility>

Equity::Equity(std::string ticker, std::uint16_t share_number, std::float_t price, std::float_t mu, std::float_t sigma)
    : s_ticker{std::move( ticker )} // transfers ownership of the string t to ticker
    , i_share_number{ share_number }
    , f_price{ price }
    , f_mu{ mu }
    , f_sigma{ sigma }
{
    std::cout << "+++ Equity " << s_ticker << " created!" << "\n";
}

// getters
std::string Equity::getTicker() const
{
    return s_ticker;
}
std::uint16_t Equity::getShareNumber() const
{
    return i_share_number;
}
std::float_t Equity::getPrice() const
{
    return f_price;
}
std::float_t Equity::getMu() const
{
    return f_mu;
}
std::float_t Equity::getSigma() const
{
    return f_sigma;
}

// setters
void Equity::setTicker(const std::string& ticker)
{
    s_ticker = ticker;
}
void Equity::setShareNumber(const std::uint16_t& share_number)
{
    i_share_number = share_number;
}
void Equity::setPrice(const std::float_t& price)
{
    f_price = price;
}
void Equity::setMu(const std::float_t& mu)
{
    f_mu = mu;
}
void Equity::setSigma(const std::float_t& sigma)
{
    f_sigma = sigma;
}

// The overall value of the shares
std::float_t Equity::getSharesValue() const
{
    return f_price * i_share_number;
}
