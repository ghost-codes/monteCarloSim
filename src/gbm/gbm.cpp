#include "gbm.h"
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <random>

std::mutex rndGenmutex;
std::mutex m;
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

  // Print the random value
  // std::cout << "Random Normal Value (Z): " << Z << std::endl;

  return Z;
}

int plot(std::vector<std::shared_ptr<vecDouble>> data) {
  std::ofstream file("brownian.dat");
  if (!file) {
    std::cerr << "Error opening file!\n";
    return 1;
  }

  int id = 0;
  for (int id = 0; id < data.size(); id++) {
    int pos = 0;
    double x = 0.0, y = 0.0;
    for (auto dataIt = data[id]->begin(); dataIt != data[id]->end(); dataIt++) {
      y = *dataIt;
      x = pos;
      file << y << " " << x << " " << y << " " << id << "\n";
      pos++;
    }
    file << "\n";
  }

  file.close();
  std::cout << "Brownian motion data saved to brownian.dat\n";

  // Call Gnuplot to plot the  data
  std::ofstream gp("plot.gnuplot");
  gp << "set title '2D Brownian Motion'\n";
  gp << "set xlabel 'Step'\n";
  gp << "set ylabel 'Position'\n";
  gp << "plot 'brownian.dat' using 2:3 with lines title 'Random Walk'\n";
  gp.close();

  // system("gnuplot -p plot.gnuplot"); // Run gnuplot

  return 0;
}

GBM::GBM(double T, double N, double currentPrice, double vol, double riskFree,
         double yield)
    : d_dt(T / N), d_volatility(vol), d_riskFree(riskFree),
      d_dividendYield(yield), d_N(N), d_values(), d_initPrice(currentPrice),
      d_thread_pool(1500) {}

double GBM::getStep(double currentPrice) {
  double z = computeZ();
  double exponentialDrift =
      d_riskFree - d_dividendYield - (pow(d_dividendYield, 2) / 2);
  double volatilityTerm = d_volatility * (sqrt(d_dt) * z);
  double pw = (exponentialDrift * d_dt) + volatilityTerm;
  double result = currentPrice * exp(pw);
  return result;
}

int generatePath(GBM *gbm, std::shared_ptr<vecDouble> vec, int N) {
  for (int i = 0; i <= N; i++) {
    double nextValue = gbm->getStep((*vec)[i]);
    vec->push_back(nextValue);
  }
  return 0;
}

void GBM::compute() {
  int number_of_tasks = 1000000;
  for (int i = 0; i < number_of_tasks; i++) {
    auto tempVec = std::make_shared<vecDouble>();
    tempVec->push_back(d_initPrice);
    d_values.push_back(tempVec);
    std::cout << "id: " << i << " :" << d_values.size() << std::endl;
    d_thread_pool.AddTask(generatePath, this, d_values[i], d_N);
  }

  while (d_thread_pool.QueueSize() != 0) {
  }
  std::cout << d_values.size() << std::endl;

  plot(d_values);
}
