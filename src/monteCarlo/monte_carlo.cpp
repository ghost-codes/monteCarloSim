#include "monte_carlo.h"
#include <cmath>
#include <cstddef>
#include <iostream>
#include <memory>
#include <mutex>
#include <random>

std::mutex rndGenmutex;
std::mutex m;

std::mutex d_reduceMutex;

double computeZ() {

  // make Compute Z thread safe
  // Using lock guards
  std::lock_guard<std::mutex> lock(rndGenmutex);

  // Create a random device and seed a generator
  std::random_device rd;
  std::mt19937 generator(rd()); // Mersenne Twister PRNG

  // Define a normal distribution with mean = 0 and stddev = 1
  std::normal_distribution<double> distribution(0.0, 1.0);

  // Generate a random number Z from the normal distribution
  double Z = distribution(generator);

  return Z;
}

MonteCarlo::MonteCarlo(double T, double N, double currentPrice, double vol,
                       double riskFree, double yield)
    : d_dt(T / N), d_volatility(vol), d_riskFree(riskFree),
      d_dividendYield(yield), d_N(N), d_values(), d_initPrice(currentPrice),
      d_mean(std::make_shared<vecDouble>(N)),
      d_sum_of_squares(std::make_shared<vecDouble>(N)),
      d_currentReduceStep(std::make_shared<int>(0)) {}

double MonteCarlo::getStep(double currentPrice) {
  double z = computeZ();
  double exponentialDrift =
      d_riskFree - d_dividendYield - (pow(d_dividendYield, 2) / 2);
  double volatilityTerm = d_volatility * (sqrt(d_dt) * z);
  double pw = (exponentialDrift * d_dt) + volatilityTerm;
  double result = currentPrice * exp(pw);
  return result;
}

int MonteCarlo::generatePath(std::shared_ptr<vecDouble> vec, int N) {
  for (int i = 0; i <= N; i++) {
    double nextValue = getStep((*vec)[i]);
    vec->push_back(nextValue);
  }
  return 0;
}

const std::vector<double> MonteCarlo::values() { return *d_mean; }

void MonteCarlo::getReduceStep() {
  // std::lock_guard<std::mutex> lock(d_reduceMutex)
  // return
}
void MonteCarlo::increaseReduceStep() {}

void MonteCarlo::reducePath(std::shared_ptr<vecDouble> vec) {
  std::lock_guard<std::mutex> lock(d_reduceMutex);
  ++(*d_currentReduceStep);

  for (size_t i = 0; i < vec->size(); i++) {
    // Compute current M
    double mn1 = d_mean->at(i);
    double xn = vec->at(i);
    double m = mn1 + ((xn - mn1) / *d_currentReduceStep);

    // Compute current sum of squared diffs
    double sn1 = d_sum_of_squares->at(i);
    double s = sn1 + ((xn - mn1) * (xn - m));
    // std::cout << "Values id << " << m << std::endl;
    (*d_sum_of_squares)[i] = s;
    (*d_mean)[i] = m;
  }
}

void MonteCarlo::operator()(int id) {

  std::cout << "Called: " << id << std::endl;
  auto tempVec = std::make_shared<vecDouble>();
  tempVec->push_back(d_initPrice);

  generatePath(tempVec, d_N);
  reducePath(tempVec);
}

// int number_of_tasks = 1000000;
// for (int i = 0; i < number_of_tasks; i++) {
//   auto tempVec = std::make_shared<vecDouble>();
//   tempVec->push_back(d_initPrice);
//   d_values.push_back(tempVec);
//   std::cout << "id: " << i << " :" << d_values.size() << std::endl;
//   d_thread_pool.AddTask(generatePath, this, d_values[i], d_N);
// }

// while (pool.QueueSize() != 0) {
// }
// std::cout << d_values.size() << std::endl;

// plot(d_values);
