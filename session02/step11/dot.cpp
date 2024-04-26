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

void copy(std::size_t n, const double* x, double* y) {
  #pragma omp parallel for
  for (std::size_t i = 0; i < n; i++) {
    y[i] = x[i];
  }
}


double dot_mimd(std::size_t n, const double* x, const double* y) {
  double result = 0;
  #pragma omp parallel for reduction(+:result)
  for (std::size_t i = 0; i < n; i++) {
    result += x[i] * y[i];
  }
  return result;
}


double dot_simd(std::size_t n, const double* x, const double* y) {
  double result = 0;
  #pragma omp simd reduction(+:result)
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
  std::vector<double> x1(len);
  std::vector<double> y1(len);
  auto t1 = now();
  rand(len, &x1[0]);
  rand(len, &y1[0]);
  auto t2 = now();
  std::vector<double> x2(len);
  std::vector<double> y2(len);
  auto t3 = now();
  copy(len, &x1[0], &x2[0]);
  copy(len, &y1[0], &y2[0]);
  auto t4 = now();
  auto result_mimd = dot_mimd(len, &x1[0], &y1[0]);
  auto t5 = now();
  auto result_simd = dot_simd(len, &x2[0], &y2[0]);
  auto t6 = now();

  std::cout << "dot product mimd: " << result_mimd << std::endl;
  std::cout << "dot product simd: " << result_simd << std::endl;

  std::cout << "time required for x1, y1 construction:        " << elapsed_time_in_seconds(t0, t1) << "s"  << std::endl;
  std::cout << "time required for x1, y1 randomization:       " << elapsed_time_in_seconds(t1, t2) << "s"  << std::endl;
  std::cout << "time required for x2, y2 construction:        " << elapsed_time_in_seconds(t2, t3) << "s"  << std::endl;
  std::cout << "time required for copying x1 to x2, y1 to y2: " << elapsed_time_in_seconds(t3, t4) << "s"  << std::endl;
  std::cout << "time required for dot_mimd:                   " << elapsed_time_in_seconds(t4, t5) << "s"  << std::endl;
  std::cout << "time required for dot_simd:                   " << elapsed_time_in_seconds(t5, t6) << "s"  << std::endl;
}