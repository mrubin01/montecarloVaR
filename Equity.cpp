#include "Equity.h"
#include <string>
#include <iostream>

Equity::Equity(std::string_view ticker, std::uint16_t share_number, std::float_t price)
    : s_ticker{ ticker }
    , i_share_number{ share_number }
    , f_price{ price }
{
    std::cout << "+++ Equity " << s_ticker << " created!" << "\n";
}

// getters
std::string_view Equity::getTicker() const
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


// setters
void Equity::setTicker(const std::string_view& ticker)
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

// The overall value of the shares
std::float_t Equity::getSharesValue() const
{
    return f_price * i_share_number;
}

