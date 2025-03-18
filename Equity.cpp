#include "Equity.h"
#include <vector>
#include <string>
#include <iostream>

Equity::Equity(std::string_view ticker, std::uint16_t share_number, std::float_t value, std::float_t price)
    : s_ticker{ ticker }
    , i_share_number{ share_number }
    , f_value{ value }
    , f_price{ price }
{
    std::cout << "Equity " << s_ticker << " created!" << "\n";
}


