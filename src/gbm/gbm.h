#ifndef GBM_H
#define GBM_H

#include <vector>

using vecDouble = std::vector<double>;

class GBM
{
public:
    GBM(double T, double N, double currentPrice, double vol, double riskFree, double yield);

    double getStep(double currentPrice);

    void compute();

private:
    double d_dt;
    double d_volatility;
    double d_riskFree;
    double d_dividendYield;
    double d_initPrice;
    int d_N;
    std::vector<vecDouble> d_values;

    void generatePath(int id);
};

#endif