#include <iostream>
#include <thread>

void foo() {
  std::cout << "foo\n";
}

void bar() {
  std::cout << "bar\n";
}

int main() {
  std::thread t1(foo);
  std::thread t2(bar);

  t1.join();
  t2.join();
}