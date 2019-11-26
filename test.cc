#include "skiplist.h"

#include <chrono>
#include <iostream>

int main(int argc, char const *argv[]) {
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