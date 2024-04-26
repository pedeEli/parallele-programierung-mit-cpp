#include <cstdlib>
#include <iostream>
#include <vector>
#include <chrono>
#include <random>


auto now() {
	return std::chrono::high_resolution_clock::now();
}

auto elapsed_time_in_seconds(auto t0, auto t1) {
	using namespace std::chrono;
	return duration<double, seconds::period>(t1 - t0).count();
}

void init(std::size_t n, double* x, double val) {
	#pragma omp parallel for
	for (std::size_t i = 0; i < n; ++i) {
		x[i] = val;
	}
}

void rand(std::size_t n, double* x) {
  #pragma omp parallel
  {
    auto seed = std::random_device()();
    thread_local std::mt19937 mt {seed};
    std::uniform_real_distribution<double> uniform(-1, 1);
    #pragma omp for
    for (std::size_t i = 0; i < n; i++) {
      x[i] = uniform(mt);
    }
  }
}

void axpy(std::size_t n, double alpha, const double* x, double* y) {
	if (alpha) {
		#pragma omp parallel for
		for (std::size_t i = 0; i < n; ++i) {
	y[i] += alpha * x[i];
		}
	}
}

int main(int argc, char* argv[]) {
	const char* cmdname = *argv++;
	--argc;
	std::size_t len = 1<<16;

	if (argc > 0 && **argv) {
		char* endptr;
		len = std::strtoull(*argv, &endptr, 0);
		if (len == 0 || *endptr) {
			std::cerr << cmdname << ": invalid length: " << *argv << std::endl;
			std::exit(1);
		}
		++argv; --argc;
	}

	if (argc > 0) {
		std::cerr << "Usage: " << cmdname << " length" << std::endl;
		std::exit(1);
	}

	auto t0 = now();
	std::vector<double> x(len);
	std::vector<double> y(len);
  auto t1 = now();
	rand(len, &x[0]);
	rand(len, &y[0]);

	auto t2 = now();
	axpy(len, 2.0, &x[0], &y[0]);
	auto t3 = now();

	std::cout << y[len-1] << std::endl;
  std::cout << "time required for vector construction: " << elapsed_time_in_seconds(t0, t1) << "s" << std::endl;
	std::cout << "time required for initialization: " << elapsed_time_in_seconds(t1, t2) << "s" << std::endl;
	std::cout << "time required for axpy: " << elapsed_time_in_seconds(t2, t3) << "s" << std::endl;
}