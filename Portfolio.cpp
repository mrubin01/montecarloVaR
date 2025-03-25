#include <string>
#include <iostream>
#include "Portfolio.h"
#include <iomanip>

Portfolio::Portfolio(std::initializer_list<Equity> equity_list)
    : v_equities{equity_list}
{
    std::cout << "+++ New Portfolio created!" << "\n";
};

void Portfolio::addEquity(const Equity& equity)
{
    v_equities.push_back(equity);
    std::cout << "+++ Added equity " << equity.getTicker() << " to portfolio!" << '\n';
};

void Portfolio::removeEquity(const std::string& ticker)
{
    v_equities.erase(std::remove_if(v_equities.begin(), v_equities.end(),[&](const Equity& e)
        { return e.getTicker() == ticker; }), v_equities.end());
    std::cout << "--- Equity " << ticker << " removed from the portfolio!" << '\n';
}

std::float_t Portfolio::getPortfolioValue() const
{
    std::float_t total = 0.0f;
    for (const auto& equity : v_equities)
    {
        total += equity.getSharesValue();
    }

    // Ensure fixed-point notation (no scientific notation)
    // std::cout << std::fixed << std::setprecision(2);
    return total;
    // std::cout << "=== Overall Portfolio value: $" << total << '\n';
}

std::vector<Equity>& Portfolio::getEquities()
{
    return v_equities;
}

// print portfolio item names
void Portfolio::printPortfolio() const
{
    for (const auto& equity : v_equities)
        std::cout << equity.getTicker() << " - " << equity.getPrice() << '\n';
}

// return the number of items in the portfolio
size_t Portfolio::getItemCount() const
{
    return v_equities.size();
}


// Add begin() and end() to support range-based loops
std::vector<Equity>::const_iterator Portfolio::begin() const
{
    return v_equities.begin();
};
std::vector<Equity>::const_iterator Portfolio::end() const
{
    return v_equities.end();
};