#include <iostream>
#include <vector>
#include <random>
#include <chrono>

#include "dynamic_functions.h"

using namespace Casper;

int main(int argc, char *argv[]) {
  std::vector<std::string> v = {"cosine"};
  DynamicFunctions dn("dynamic_functions_source/dynamic_test_function.so", v);

  const double lower_bound = 0;
  const double upper_bound = 1;
  std::uniform_real_distribution<double> unif(lower_bound, upper_bound);
  std::random_device rand_dev;
  std::mt19937 rand_engine(rand_dev());

  while (true) {
    double x = unif(rand_engine);
    double result = dn.call<double>("cosine", x);
    std::cout << "Result of calling cos(" << x << "): " << result << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
}
