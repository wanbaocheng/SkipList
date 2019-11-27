#include "skiplist.h"

#include <chrono>
#include <cstdio>
#include <iostream>
#define Logf(fmt, ...) fprintf(stderr, fmt "\n", __VA_ARGS__)

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

  // sl.Insert(1);
  // sl.Insert(1);
  // sl.Insert(1);
  // sl.Erase(sl.Find(1));
  // sl[0];
  // sl[1];

  // auto t1_ = std::chrono::steady_clock::now();
  for (int i = 0; i < 10; ++i) {
    sl.Insert(random() % 10);
  }

  sl.Erase(sl.Find(5));
  sl.Erase(sl.Find(9));
  for (int i = 0; i < 8; ++i) {
    Log("***  %d->", sl[i]);
  }

  // sl.Dump();

  // sl.Erase(sl.Find(1));

  // sl.Dump();

  // sl[0];
  // sl[1];

  // for (int i = 0; i < 2; ++i) {
  //   Log("***  %d->", sl[i]);
  // }

  // sl.Erase(sl.Find(1));
  // assert(sl.Size() == 3);
  // for (int i = 0; i < 3; ++i) {
  //   Log("%d->", sl[i]);
  // }

  // auto t2_ = std::chrono::steady_clock::now();
  // int ms =
  //     std::chrono::duration_cast<std::chrono::milliseconds>(t2_ -
  //     t1_).count();
  // std::cout << " timespan=" << ms << "ms"
  //           << "\n";
  return 0;
}