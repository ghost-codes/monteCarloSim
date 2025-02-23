
#include "monte_carlo.h"
#include "thread_pool.h"
#include <fstream>
#include <iostream>

int plot(vecDouble data) {
  std::ofstream file("brownian.dat");
  if (!file) {
    std::cerr << "Error opening file!\n";
    return 1;
  }

  int id = 0;
  for (int id = 0; id < data.size(); id++) {
    int pos = 0;
    double x = id, y = data[id];

    file << id << " " << x << " " << y << "\n";
    pos++;

    // file << "\n";
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

int main(int argc, char **argv) {

  ThreadPool pool = ThreadPool{1500};

  MonteCarlo *simulator = new MonteCarlo(1, 252, 261.48, 0.25, 0.0431, 0.0255);
  // simulator.compute();

  int number_of_tasks = 1000;
  for (int i = 0; i < number_of_tasks; i++) {
    pool.AddTask(*simulator, i);
  }

  while (!pool.isPoolDone()) {
  }

  plot(simulator->values());
  return 0;
}

// std::unordered_map<std::string, std::string> u =
//     {
//         {"-s", "Initial Price"},
//         {"-u", "Drift"},
//         {"-v", "Volatility"},
//         {"-t", "Time Horizon"},
//         {"-n", "Number of steps"},
//         {"-m", "Number of paths (Optional)"}};

// InputVariables vars;

// for (int i = 1; i < argc; ++i)
// {
//     const char *key = argv[i];
//     if (strcmp("-s", key) == 0)
//     {
//         vars.d_initialPrice = std::stoi(argv[++i]);
//         std::cout << vars.d_initialPrice << std::endl;
//     }
// }