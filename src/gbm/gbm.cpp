#include "gbm.h"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <random>

double computeZ() {

  // make Compute Z thread safe
  // Using lock guards

  // Create a random device and seed a generator
  std::random_device rd;
  std::mt19937 generator(rd()); // Mersenne Twister PRNG

  // Define a normal distribution with mean = 0 and stddev = 1
  std::normal_distribution<double> distribution(0.0, 1.0);

  // Generate a random number Z from the normal distribution
  double Z = distribution(generator);

  // Print the random value
  std::cout << "Random Normal Value (Z): " << Z << std::endl;

  return Z;
}

int plot(std::vector<vecDouble> data) {
  std::ofstream file("brownian.dat");
  if (!file) {
    std::cerr << "Error opening file!\n";
    return 1;
  }

  int id = 0;
  for (int id = 0; id < data.size(); id++) {
    int pos = 0;
    double x = 0.0, y = 0.0;
    for (auto dataIt = data[id].begin(); dataIt != data[id].end(); dataIt++) {
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

  system("gnuplot -p plot.gnuplot"); // Run gnuplot

  return 0;
}

GBM::GBM(double T, double N, double currentPrice, double vol, double riskFree,
         double yield)
    : d_dt(T / N), d_volatility(vol), d_riskFree(riskFree),
      d_dividendYield(yield), d_N(N), d_values(), d_initPrice(currentPrice) {}

double GBM::getStep(double currentPrice) {
  double z = computeZ();
  double exponentialDrift =
      d_riskFree - d_dividendYield - (pow(d_dividendYield, 2) / 2);
  double volatilityTerm = d_volatility * (sqrt(d_dt) * z);
  double pw = (exponentialDrift * d_dt) + volatilityTerm;
  double result = currentPrice * exp(pw);
  return result;
}

void GBM::generatePath(int id) {
  std::cout << d_dt << " " << d_values[id][0] << std::endl;
  std::cout << "Index 0: " << d_values[id][0] << std::endl;
  for (int i = 0; i <= d_N; i++) {
    double nextValue = getStep(d_values[id][i]);
    d_values[id].push_back(nextValue);
  }
}

void GBM::compute() {
  int number_of_threads = 1000000;
  for (int i = 0; i < number_of_threads; i++) {
    auto tempVec = std::vector<double>();
    tempVec.push_back(d_initPrice);
    d_values.push_back(tempVec);
    generatePath(i);
  }

  plot(d_values);
}
