# SkipList
SkipList implemented in c++11

# Features
* c++11 implementation.
* The time complexity of element inserts, queries, and deletes is O(logn).
* You can use it for ranking, the time complexity of both query elements by rank and query rank by elements is also O(logn).

# API
```C++
SkipList:
void Insert(const T& data);
void Insert(T&& data);
Iterator Find(const T& data) const;
const T& At(const int index) const;
const T& operator[](const int index) const;
int IndexOf(const T& data) const;
bool Empty() const;
int Size() const;
bool Erase(Iterator& erase_it);
bool Erase(Iterator&& erase_it);

Iterator:
bool operator==(const Iterator& other) const;
bool operator!=(const Iterator& other);
Iterator& operator++();
Iterator operator++(int);
const T& operator*();
const T* operator->();
```

# Usage
```C++
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
```

# Benchmark
Tested on my i5-7500 3.40GHz, see [test](test.cc).

```
10000 elements random insert,timespan=6
10000 elements random query by index,timespan=2
10000 elements random query by data,timespan=2
10000 elements random query index of data,timespan=2
10000 elements random delete,timespan=9
------------------------------------------------------------------
100000 elements random insert,timespan=102
100000 elements random query by index,timespan=60
100000 elements random query by data,timespan=70
100000 elements random query index of data,timespan=70
100000 elements random delete,timespan=100
------------------------------------------------------------------
1000000 elements random insert,timespan=1847
1000000 elements random query by index,timespan=1354
1000000 elements random query by data,timespan=1622
1000000 elements random query index of data,timespan=1652
1000000 elements random delete,timespan=1880
------------------------------------------------------------------
```
