#include "skiplist.h"

#include <chrono>
#include <iostream>

class Test {
 public:
  Test() {
    std::cout << "0000000000"
              << "\n";
  }
  ~Test() {
    std::cout << "111111111111"
              << "\n";
  }
  Test(const Test& other) {
    std::cout << "222222222222"
              << "\n";
  };
  Test(Test&& other) {
    std::cout << "333333333"
              << "\n";
  };
  bool operator<(const Test& other) const { return true; }
};

int main(int argc, char const* argv[]) {
  (void)argc;
  (void)argv;

  SkipList<int> sl;

  auto t1_ = std::chrono::steady_clock::now();
  for (int i = 0; i < 1000000; ++i) {
    sl.Insert(random() % 1000000);
  }

  auto t2_ = std::chrono::steady_clock::now();
  int ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(t2_ - t1_).count();

  std::cout << " timespan=" << ms << "ms"
            << "\n";
  return 0;
}