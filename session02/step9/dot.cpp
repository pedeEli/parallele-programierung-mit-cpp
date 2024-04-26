#include <chrono>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <random>
#include <vector>

void rand(std::size_t n, double* x) {
  #pragma omp parallel
  {
    auto seed = std::random_device()();
    thread_local std::mt19937 mt{seed};
    std::uniform_real_distribution<double> uniform(-1, 1);

    #pragma omp for
    for (std::size_t i = 0; i < n; ++i) {
      x[i] = uniform(mt);
    }
  }
}


double dot(std::size_t n, const double* x, const double* y) {
  double result = 0;
  #pragma omp parallel for reduction(+:result)
  for (std::size_t i = 0; i < n; i++) {
    result += x[i] * y[i];
  }
  return result;
}

auto now() {
  return std::chrono::high_resolution_clock::now();
}
auto elapsed_time_in_seconds(auto t0, auto t1) {
  using namespace std::chrono;
  return duration<double, seconds::period>(t1 - t0).count();
}

int main(int argc, char* argv[]) {
  const char* cmdname = *argv++;
  --argc;
  std::size_t len = 1 << 16;

  if (argc > 0 && **argv) {
    char* endptr;
    len = std::strtoull(*argv, &endptr, 0);
    if (len == 0 || *endptr) {
      std::cerr << cmdname << ": invalid length: " << *argv << std::endl;
      std::exit(1);
    }
    ++argv;
    --argc;
  }

  if (argc > 0) {
    std::cerr << "Usage: " << cmdname << " length" << std::endl;
    std::exit(1);
  }

  auto t0 = now();
  std::vector<double> x(len);
  auto t1 = now();
  rand(len, &x[0]);

  auto t2 = now();
  auto result = dot(len, &x[0], &x[0]);
  auto t3 = now();

  std::cout << "dot product: " << result << std::endl;

  std::cout << "time required for vector construction: " << elapsed_time_in_seconds(t0, t1) << " s"  << std::endl;
  std::cout << "time required for initialization:      " << elapsed_time_in_seconds(t1, t2) << " s"  << std::endl;
  std::cout << "time required for dot:                 " << elapsed_time_in_seconds(t2, t3) << " s"  << std::endl;
}