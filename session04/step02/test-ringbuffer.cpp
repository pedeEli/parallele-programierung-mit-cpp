#include <iostream>
#include <sstream>
#include <thread>
#include "ringbuffer.hpp"

int main() {
  Ringbuffer<int> rb(4);

  std::thread producer([&rb]() {
    for (int i = 0; i < 100; ++i) {
      if (!rb.write(i)) {
        break;
      }
      std::ostringstream os;
      os << "producing " << i << std::endl;
      std::cout << os.str();
    }
    rb.done();
  });

  std::thread consumer([&rb]() {
    for(;;) {
      auto item = rb.read();
      if (!item) {
        break;
      }
      std::ostringstream os;
      os << "consuming " << item.value() << std::endl;
      std::cout << os.str();
    }
  });

  producer.join();
  consumer.join();
}