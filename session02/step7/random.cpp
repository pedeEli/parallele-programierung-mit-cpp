#include <iostream>
#include <random>

int main() {
  auto seed = std::random_device()();
  std::mt19937 mt{seed};
  std::uniform_real_distribution<double> uniform(-1, 1);
  
  for (int i = 0; i < 10; ++i) {
    std::cout << uniform(mt) << std::endl;
  }
}