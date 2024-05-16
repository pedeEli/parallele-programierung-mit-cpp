#include <iostream>
#include <sstream>
#include <thread>
#include <vector>


void foo(unsigned int i) {
  std::ostringstream os;
  os << i << std::endl;
  std::cout << os.str();
}


int main() {
  auto nof_threads = std::thread::hardware_concurrency();
  std::vector<std::thread> threads(nof_threads);

  for (unsigned int i = 0; i < nof_threads; i++) {
    threads[i] = std::thread(foo, i);
  }

  for (auto& t : threads) {
    t.join();
  }
}