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

  // SkipList<int> sl;
  // auto t1_ = std::chrono::steady_clock::now();
  // for (int i = 0; i < 1000000; ++i) {
  //   sl.Insert(random() % 1000000);
  // }
  // auto t2_ = std::chrono::steady_clock::now();
  // int ms =
  //     std::chrono::duration_cast<std::chrono::milliseconds>(t2_ -
  //     t1_).count();
  // std::cout << " timespan=" << ms << "ms"
  //           << "\n";

  // for (int i = 0; i < 100; ++i) {
  //   sl.Erase(sl.Find(sl[random() % 100]));
  // }

  // auto t3_ = std::chrono::steady_clock::now();
  // for (int i = 0; i < 1000000 - 100; ++i) {
  //   sl[i];
  // }
  // auto t4_ = std::chrono::steady_clock::now();
  // ms = std::chrono::duration_cast<std::chrono::milliseconds>(t4_ -
  // t3_).count(); std::cout << " timespan=" << ms << "ms"
  //           << "\n";

  // auto t5_ = std::chrono::steady_clock::now();
  // for (auto it = sl.Begin(); it != sl.End(); ++it) {
  //   *it;
  // }
  // auto t6_ = std::chrono::steady_clock::now();
  // ms = std::chrono::duration_cast<std::chrono::milliseconds>(t6_ -
  // t5_).count(); std::cout << " timespan=" << ms << "ms"
  //           << "\n";

  // LeetCode:
  // 1206. Design Skiplist
  class Skiplist {
   public:
    Skiplist() {}

    bool search(int target) { return sl_.Find(target) != sl_.End(); }

    void add(int num) { sl_.Insert(num); }

    bool erase(int num) {
      return sl_.Erase(sl_.Find(num));
    }

   private:
    SkipList<int> sl_;
  };

  Skiplist slleetcode;
  slleetcode.add(1);
  // slleetcode.dump();
  slleetcode.add(2);
  slleetcode.add(3);

  // Log("search1 %d,", slleetcode.search(1));

  Log("search0 %d,", slleetcode.search(0));
  slleetcode.add(4);
  Log("search1 %d,", slleetcode.search(1));

  Log("erase0 %d,", slleetcode.erase(0));
  Log("erase1 %d,", slleetcode.erase(1));
  Log("search1 %d,", slleetcode.search(1));

  return 0;
}