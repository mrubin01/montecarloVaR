#pragma once
#include <string>

class Equity
{
private:
    std::string_view s_ticker{};
    std::uint16_t i_share_number{};
    std::float_t f_price{};
public:
    Equity() = default; // is an explicit default constructor useful?

    Equity(std::string_view ticker,
        std::uint16_t share_number,
        std::float_t price);

    // getters
    std::string_view getTicker() const;
    std::uint16_t getShareNumber() const;
    std::float_t getPrice() const;

    // setters
    void setTicker(const std::string_view& ticker);
    void setShareNumber(const std::uint16_t& share_number);
    void setPrice(const std::float_t& price);

    // get the overall value of the shares
    std::float_t getSharesValue() const;

};

