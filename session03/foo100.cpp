#include <iostream>
#include <sstream>
#include <vector>
#include <thread>


void foo(unsigned int start, unsigned int end) {
  std::ostringstream os;
  os << "start: " << start << ", end: " << end << std::endl;
  std::cout << os.str();
}


int main() {
  auto nof_threads = std::thread::hardware_concurrency();
  std::cout << "nof_threads: " << nof_threads << std::endl;
  std::vector<std::thread> threads(nof_threads);
  unsigned int start = 0;
  unsigned int end = 90;

  auto div = (end - start) / nof_threads;
  auto remainder = (end - start) % nof_threads;

  for (unsigned i = 0; i < nof_threads; i++) {
    auto e = start + div;
    if (remainder > 0) {
      e++;
      remainder--;
    }

    threads[i] = std::thread(foo, start, e);
    start = e;
  }

  for (auto& thread : threads) {
    thread.join();
  }
}