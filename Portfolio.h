#pragma once
#include <string>
#include <vector>
#include "Equity.h"

class Portfolio
{
private:
    std::vector<Equity> v_equities{};
public:
    // default constructor
    Portfolio() = default;

    // constructor accepting one or more equities
    Portfolio(std::initializer_list<Equity> equity_list);

    // getters
    // get the overall value of the portfolio
    std::float_t getPortfolioValue() const;
    // get the assets in the portfolio
    std::vector<Equity>& getEquities();
    // get the number of items in the portfolio
    size_t getItemCount() const;


    // add a new equity
    void addEquity(const Equity& equity);

    // remove an equity
    void removeEquity(const std::string& ticker);


    // print portfolio item names
    void printPortfolio() const;

    // Add begin() and end() to support range-based loops
    std::vector<Equity>::const_iterator begin() const;
    std::vector<Equity>::const_iterator end() const;
};