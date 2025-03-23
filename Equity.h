#pragma once
#include <string>

class Equity
{
private:
    std::string s_ticker{};
    std::uint16_t i_share_number{};
    std::float_t f_price{};
    std::float_t f_mu{};
    std::float_t f_sigma{};
public:
    Equity() = default; // is an explicit default constructor useful?

    Equity(std::string ticker,
        std::uint16_t share_number,
        std::float_t price,
        std::float_t mu,
        std::float_t sigma);

    // getters
    std::string getTicker() const;
    std::uint16_t getShareNumber() const;
    std::float_t getPrice() const;
    std::float_t getMu() const;
    std::float_t getSigma() const;

    // setters
    void setTicker(const std::string& ticker);
    void setShareNumber(const std::uint16_t& share_number);
    void setPrice(const std::float_t& price);
    void setMu(const std::float_t& mu);
    void setSigma(const std::float_t& sigma);

    // get the overall value of the shares
    std::float_t getSharesValue() const;

};

