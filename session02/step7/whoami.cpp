#include <iostream>
#include <sstream>

#include <omp.h> // OpenMP library functions

int main() {
  std::cout << "initial number of threads: " << omp_get_num_threads() << std::endl;

  #pragma omp parallel
  {
    /* make sure that the output lines do not mix */
    std::ostringstream os;
    os << "thread " << omp_get_thread_num() << " of " << omp_get_num_threads() << " is active" << std::endl;
    /* this is thread-safe as long as std::cout is tied to stdout of C which is thread-safe */
    std::cout << os.str();
  }
}