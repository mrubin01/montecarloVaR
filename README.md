## Montecarlo VaR - C++
This program calculates the Value at Risk (VaR) for one or more equities using the Monte Carlo simulations.
Moreover, it calculates the Expected Shortfall (ES). 
VaR and ES are both calculated with two different confidence level (95% and 99%). 
If the portfolio is composed of just one ticker, the Cholesky decomposition is not necessary

The sample portfolio is composed of 3 tickers: AAPL, CIM, CVX. They have a weak or no correlation at all. 
The number of simulations is 1000, the trading days is 5. 
The data will be loaded from csv files. 

## Steps
1. Fetch data from the csv files having the fields Date,Close,Returns,Log Returns
2. From the Log Returns ccalculate mean (mu) and std (sigma)
6. In case of one ticker, create an empty matrix and fill it with random prices having a normal distribution
   In case of more tickers, create an empty tensor and fill it with random prices having a normal distribution
7. In both cases, the random prices are generated using the Geometric Brownian Motion
8. Compute the Value at Risk using the last simulation in the matrix/tensor with confidence interval 95% and 99%
10. Compute the Expected Shortfall, that is the the average loss in the worst-case scenarios (beyond the confidence threshold)
11. Print VaR and ES with 95% and 99% confidence level
