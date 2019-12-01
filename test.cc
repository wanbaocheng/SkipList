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
    (void)index;
  }

  auto t5 = std::chrono::steady_clock::now();
  // random delete
  for (int i = 0; i < max_size; ++i) {
    sl.Erase(sl.Find(nums[i]));
  }
  auto t6 = std::chrono::steady_clock::now();

  assert(sl.Size() == 0);

  Log("%d elements random insert,timespan=%ldms\n", max_size,
      std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count());
  Log("%d elements random query by index,timespan=%ldms\n", max_size,
      std::chrono::duration_cast<std::chrono::milliseconds>(t3 - t2).count());
  Log("%d elements random query by data,timespan=%ldms\n", max_size,
      std::chrono::duration_cast<std::chrono::milliseconds>(t4 - t3).count());
  Log("%d elements random query index of data,timespan=%ldms\n", max_size,
      std::chrono::duration_cast<std::chrono::milliseconds>(t5 - t4).count());
  Log("%d elements random delete,timespan=%ldms\n", max_size,
      std::chrono::duration_cast<std::chrono::milliseconds>(t6 - t5).count());
  Log("%s\n",
      "------------------------------------------------------------------");
  delete[] nums;
}

void Usage() {
  class Foo {
   public:
    Foo() {}  // requires default constructor
    Foo(const Foo& other) { num_ = other.num_; }  // requires copy constructor

    ~Foo() {}
    Foo(int num) : num_(num) {}
    int get_num() const { return num_; }

    bool operator<(const Foo& other) const {  // requires less operator
      return num_ < other.num_;
    }

   private:
    int num_;
  };

  struct Compare {
    bool operator()(const Foo& foo, const Foo& other) const {
      return foo.get_num() < other.get_num();
    }
  };

  SkipList<Foo> sl;            // use operator< to compare
  SkipList<Foo, Compare> sl0;  // optional use Compare Object

  assert(sl.Empty());

  // Insert O(logn)
  Foo foo(1);
  sl.Insert(foo);     // copy constructor
  sl.Insert(Foo(2));  // move copy constructor if exist
  sl.Insert(Foo(6));
  sl.Insert(Foo(4));
  assert(sl.Size() == 4);
  assert(!sl.Empty());

  // Query O(logn)
  // Query by data, return iterator
  auto it = sl.Find(foo);
  assert(it->get_num() == 1);
  assert((*it).get_num() == 1);
  assert((it++)->get_num() == 1);
  assert(it->get_num() == 2);
  assert(++it == sl.Find(Foo(4)));
  it = sl.Find(Foo(10));
  assert(it == sl.End());
  it = sl.FindFirstGreater(Foo(4));
  assert(it->get_num() == 6);
  it = sl.FindLastLess(Foo(4));
  assert(it->get_num() == 2);

  // Query by data, return index
  assert(sl.IndexOf(foo) == 0);
  assert(sl.IndexOf(Foo(2)) == 1);

  // Query by index, return const T&
  assert(sl.At(0).get_num() == sl[0].get_num());

  // for data range 2->6
  for (it = sl.Find(2); it != sl.Find(6); ++it) {
    assert(it->get_num() == (*it).get_num());
  }

  // for data range all
  for (it = sl.Begin(); it != sl.End(); ++it) {
    assert(it->get_num() == (*it).get_num());
  }

  // for index range (low efficient, O(nlog))
  for (int i = 1; i < 4; ++i) {
    assert(sl[i].get_num() == sl.At(i).get_num());
  }

  // for index range (high efficient, O(n))
  auto begin_it = sl.Find(sl[1]);
  auto end_it = sl.Find(sl[3]);
  for (it = begin_it; it != end_it; ++it) {
    assert(it->get_num() == (*it).get_num());
  }

  // Delete O(logn)
  it = sl.Find(foo);
  sl.Erase(it);
  assert(it->get_num() == 2);
  it = sl.Find(Foo(6));
  sl.Erase(it);
  assert(it == sl.End());

  assert(sl.Size() == 2);
  int deleted = 0;
  // Conditional deletion
  for (it = sl.Begin(); it != sl.End();) {
    if (random() % 2 == 0) {
      sl.Erase(it);
      ++deleted;
    } else {
      ++it;
    }
  }
  assert(sl.Size() == (2 - deleted));
}

int main(int argc, char const* argv[]) {
  (void)argc;
  (void)argv;

  Usage();

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