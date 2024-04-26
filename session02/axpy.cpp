#include <cstdlib>
#include <vector>
#include <iostream>

void axpy(std::size_t n, double alpha, const double* x, double* y) {
	#pragma omp parallel for
	for (std::size_t i = 0; i < n; i++) {
		y[i] = alpha * x[i] + y[i];
	}
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
		++argv; --argc;
	}

	if (argc > 0) {
		std::cerr << "Usage: " << cmdname << " length" << std::endl;
		std::exit(1);
	}

	std::vector<double> x(len, 10);
	std::vector<double> y(len, 20);
	axpy(len, 2, &x[0], &y[0]);
	std::cout << y[len-1] << std::endl;
}