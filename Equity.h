#pragma once
#include <string>

class Equity
{
private:
    std::string_view s_ticker{};
    std::uint16_t i_share_number{};
    std::float_t f_value{};
    std::float_t f_price{};
public:
    Equity() = default; // is an explicitly default constructor useful?

    Equity(std::string_view ticker,
        std::uint16_t share_number,
        std::float_t value,
        std::float_t price);

    std::string_view getTicker() const {return s_ticker;}
    std::uint16_t getShareNumber() const {return i_share_number;}
    std::float_t getValue() const {return f_value;}
    std::float_t getPrice() const {return f_price;}

};

