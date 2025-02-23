#ifndef GBM_H
#define GBM_H

#include <memory>
#include <mutex>
#include <vector>

using vecDouble = std::vector<double>;

class MonteCarlo {
public:
  MonteCarlo(double T, double N, double currentPrice, double vol,
             double riskFree, double yield);

  double getStep(double currentPrice);
  const std::vector<double> values();
  void compute();
  int generatePath(std::shared_ptr<vecDouble> vec, int N);
  void operator()(int id);
  void reducePath(std::shared_ptr<vecDouble> vec);
  void getReduceStep();
  void increaseReduceStep();

private:
  double d_dt;
  double d_volatility;
  double d_riskFree;
  double d_dividendYield;
  double d_initPrice;
  int d_N;
  std::shared_ptr<std::vector<double>> d_values;
  std::shared_ptr<std::vector<double>> d_mean;
  std::shared_ptr<std::vector<double>> d_sum_of_squares;

  std::shared_ptr<int> d_currentReduceStep;

  static int generatePath(MonteCarlo *gbm, int id);
};

#endif