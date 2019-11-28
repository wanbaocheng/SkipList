#include "skiplist.h"

#include <cassert>
#include <chrono>
#include <cstdio>
#include <iostream>

#define Log(fmt, ...) fprintf(stderr, fmt, __VA_ARGS__)

// return random = [min, max]
int random_range(int min, int max) { return random() % (max - min + 1) + min; }

void Test(const int max_size) {
  int* nums = new int[max_size];
  for (int i = 0; i < max_size; ++i) {
    nums[i] = i;
  }

  for (int i = 0; i < max_size - 1; ++i) {
    int random_index = random_range(i + 1, max_size - 1);
    int temp = nums[random_index];
    nums[random_index] = nums[i];
    nums[i] = temp;
  }

  SkipList<int> sl;

  auto t1 = std::chrono::steady_clock::now();
  // random insert
  for (int i = 0; i < max_size; ++i) {
    sl.Insert(nums[i]);
  }

  auto t2 = std::chrono::steady_clock::now();
  // random query by index
  for (int i = 0; i < max_size; ++i) {
    int x = sl[random() % max_size];
    (void)x;
  }

  auto t3 = std::chrono::steady_clock::now();
  // random query by data
  for (int i = 0; i < max_size; ++i) {
    SkipList<int>::Iterator it = sl.Find(nums[i]);
  }

  auto t4 = std::chrono::steady_clock::now();
  // random query index of data
  for (int i = 0; i < max_size; ++i) {
    int index = sl.IndexOf(nums[i]);
  }

  auto t5 = std::chrono::steady_clock::now();
  // random delete
  for (int i = 0; i < max_size; ++i) {
    sl.Erase(sl.Find(nums[i]));
  }
  auto t6 = std::chrono::steady_clock::now();

  assert(sl.Size() == 0);

  Log("%d elements random insert,timespan=%ld\n", max_size,
      std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count());
  Log("%d elements random query by index,timespan=%ld\n", max_size,
      std::chrono::duration_cast<std::chrono::milliseconds>(t3 - t2).count());
  Log("%d elements random query by data,timespan=%ld\n", max_size,
      std::chrono::duration_cast<std::chrono::milliseconds>(t4 - t3).count());
  Log("%d elements random query index of data,timespan=%ld\n", max_size,
      std::chrono::duration_cast<std::chrono::milliseconds>(t5 - t4).count());
  Log("%d elements random delete,timespan=%ld\n", max_size,
      std::chrono::duration_cast<std::chrono::milliseconds>(t6 - t5).count());
  Log("%s\n", "------------------------------------------------------------------");
  delete[] nums;
}

int main(int argc, char const* argv[]) {
  (void)argc;
  (void)argv;

  Test(10000);
  Test(100000);
  Test(1000000);

  // LeetCode:
  // 1206. Design Skiplist
  class Skiplist {
   public:
    Skiplist() {}

    bool search(int target) { return sl_.Find(target) != sl_.End(); }

    void add(int num) { sl_.Insert(num); }

    bool erase(int num) { return sl_.Erase(sl_.Find(num)); }

   private:
    SkipList<int> sl_;
  };

  return 0;
}