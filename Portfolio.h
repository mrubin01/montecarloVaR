#pragma once
#include <string>
#include <vector>
#include "Equity.h"

class Portfolio
{
private:
    std::vector<Equity> v_equities;
public:
    // constructor accepting one or more equities
    Portfolio(std::initializer_list<Equity> equity_list);

    // add a new equity
    void addEquity(const Equity& equity);

    // remove an equity
    void removeEquity(const std::string_view& ticker);

    // get the overall value of the portfolio
    void getPortfolioValue() const;
};