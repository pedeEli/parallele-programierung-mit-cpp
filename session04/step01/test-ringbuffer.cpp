#include <iostream>
#include <sstream>
#include <thread>
#include "ringbuffer.hpp"

int main() {
  Ringbuffer<int> rb(4);

  std::thread producer([&rb]() {
    for (int i = 0; i < 100; ++i) {
      std::ostringstream os;
      if (rb.write(i)) {
        os << "producing " << i << std::endl;
      } else {
        os << "skipping " << i << std::endl;
      }
      std::cout << os.str();
    }
    rb.done();
  });

  std::thread consumer([&rb]() {
    while (!rb.is_finished()) {
      std::ostringstream os;
      auto item = rb.read();
      if (item.has_value()) {
        os << "consuming " << item.value() << std::endl;
      } else {
        os << "got nothing" << std::endl;
      }
      std::cout << os.str();
    }
  });
  
  producer.join();
  consumer.join();
}