#include <string>
#include <iostream>
#include "Portfolio.h"
#include <iomanip>

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

void Portfolio::removeEquity(const std::string_view& ticker)
{
    v_equities.erase(std::remove_if(v_equities.begin(), v_equities.end(),[&](const Equity& e)
        { return e.getTicker() == ticker; }), v_equities.end());
    std::cout << "--- Equity " << ticker << " removed from the portfolio!" << '\n';
}

void Portfolio::getPortfolioValue() const
{
    std::float_t total = 0.0f;
    for (const auto& equity : v_equities)
    {
        total += equity.getSharesValue();
    }

    // Ensure fixed-point notation (no scientific notation)
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "=== Overall Portfolio value: $" << total << '\n';
}
