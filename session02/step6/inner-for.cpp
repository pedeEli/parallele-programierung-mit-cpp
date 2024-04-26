#include <iostream>
#include <sstream>

#include <omp.h>

int main() {
  #pragma omp parallel
  {
    std::ostringstream os;
    #pragma omp for
    for (int i = 0; i < 100; ++i) {
      os << "[" << omp_get_thread_num() << "] processes " << i << std::endl;
      std::cout << os.str(); os.str("");
    }
  }
}