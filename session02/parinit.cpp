#include <cstdlib>
#include <iostream>
#include <vector>
#include <chrono>


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
	init(len, &x[0], 42);
	init(len, &y[0], 7);

	auto t1 = now();
	axpy(len, 2.0, &x[0], &y[0]);
	auto t2 = now();

	std::cout << y[len-1] << std::endl;
	std::cout << "time required for initialization: " << elapsed_time_in_seconds(t0, t1) << "s" << std::endl;
	std::cout << "time required for axpy: " << elapsed_time_in_seconds(t1, t2) << "s" << std::endl;
}