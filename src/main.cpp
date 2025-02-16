#include <iostream>
#include <unordered_map>
#include "input_variables.h"
#include "gbm.h"

int main(int argc, char **argv)
{
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
    GBM gbm = GBM(0.25, 100, 255.70, 0.25, 0.04532, 0.0259);
    gbm.compute();
    return 0;
}