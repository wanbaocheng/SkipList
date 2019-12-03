#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <algorithm>
#include <cassert>
#include <cstring>
#include <functional>
#include <random>

// It's almost guaranteed to be logn if the maximum number of nodes range in 0
// to 2^20
const int kMaxLevel = 20;

template <typename T, typename Comparator = std::less<T>>
class SkipList {
 private:
  class Node;

 public:
  class Iterator;

  SkipList() : head_(new Node(kMaxLevel)), compare_(), size_(0) {
    static_assert(std::is_default_constructible<T>::value,
                  "T requires default constructor");
    static_assert(std::is_copy_constructible<T>::value,
                  "T requires copy constructor");

    for (int l = 0; l < kMaxLevel; ++l) {
      head_->nexts[l] = 0;
      head_->spans[l] = 1;
    }
  }

  ~SkipList() {
    Node* p = head_;
    Node* temp;
    while (p) {
      temp = p;
      p = p->nexts[0];
      delete temp;
    }
  }

  SkipList(const SkipList&) = delete;
  SkipList(SkipList&&) = delete;
  SkipList& operator=(const SkipList& other) = delete;
  SkipList& operator=(SkipList&& other) = delete;

  // Find the first node whose data is greater than or equal to the given data,
  // then insert a new node before it.
  // The copy constructer will be used.
  void Insert(const T& data) {
    Node* new_node = new Node(RandomLevel(), data);
    InsertNode(new_node);
  }

  // Find the first node whose data is greater than or equal to the given data,
  // then insert a new node before it
  // The Move constructer will be used.
  void Insert(T&& data) {
    Node* new_node = new Node(RandomLevel(), std::move(data));
    InsertNode(new_node);
  }

  // Find the first node whose data is equals to the given data.
  // If success return the iterator, else return the end iterator.
  Iterator Find(const T& data) const {
    Node* find_node = FindNodeFirstEquals(data);
    return InternalFind(data, find_node);
  }

  // Find the first node whose data is greater than the given data.
  // If success return the iterator, else return the end iterator.
  Iterator FindFirstGreater(const T& data) const {
    Node* find_node = FindNodeFirstGreater(data);
    return Iterator(find_node);
  }

  // Find the last node whose data is less than the given data.
  // If success return the iterator, else return the end iterator.
  Iterator FindLastLess(const T& data) const {
    Node* find_node = FindNodeLastLess(data);
    return Iterator(find_node);
  }

  // Find data whose position is at index.
  const T& At(const int index) const;
  const T& operator[](const int index) const { return At(index); }

  // Find index of first node whose data is equals to the given data.
  // If data not exist return -1
  int IndexOf(const T& data) const;
  // int operator[](const T& data) const { return IndexOf(index); }

  // Find data node whose position is at index.
  bool Empty() const { return 0 == size_; };

  // Retun the size of SkipList
  int Size() const { return size_; };

  // Delete the node whose address is equals to
  // the address of the node held by given iterator.
  // If success return true and change erase_it to ++erase_it,
  // else return false(the node held by erase_it not exist).
  bool Erase(Iterator&& erase_it) { return InternalErase(erase_it); }

  // Delete the node whose address is equals to
  // the address of the node held by given iterator.
  // If success return true and change erase_it to ++erase_it,
  // else return false(the node held by erase_it not exist).
  bool Erase(Iterator& erase_it) { return InternalErase(erase_it); }

  // Delete the first node whose data is equals to give data
  // If success return true else return false
  bool Erase(const T& data);

  Iterator Begin() const { return Iterator(head_->nexts[0]); }
  Iterator End() const { return Iterator(nullptr); }

 private:
  bool InternalErase(Iterator& erase_it);

  int RandomLevel() const {
    int level = 1;
    while (level < kMaxLevel && (rand() % 2 == 0)) {
      ++level;
    }
    return level;
  }

  void InsertNode(Node* const new_node) {
    Node* prevs[kMaxLevel];
    int prev_spans[kMaxLevel];
    memset(&prev_spans[0], 0, sizeof(prev_spans[0]) * kMaxLevel);
    FindNodeFirstEquals(new_node->data, prevs, prev_spans);

    int new_level = new_node->level;
    for (int l = 0; l < new_level; ++l) {
      // Update prevs' nexts and new_node's nexts
      new_node->nexts[l] = prevs[l]->nexts[l];
      prevs[l]->nexts[l] = new_node;
      // Update prevs' spans and new_node's spans
      new_node->spans[l] = prevs[l]->spans[l] - prev_spans[l];
      prevs[l]->spans[l] = prev_spans[l] + 1;
    }

    for (int l = new_level; l < kMaxLevel; ++l) {
      // Update prevs' spans
      ++prevs[l]->spans[l];
    }
    ++size_;
  }

  Iterator Find(const T& data, Node** prevs, int* spans) const {
    Node* find_node = FindNodeFirstEquals(data, prevs, spans);
    return InternalFind(data, find_node);
  }

  Iterator InternalFind(const T& data, Node* find_node) const {
    if (nullptr != find_node && Equals(data, find_node->data)) {
      return Iterator(find_node);
    } else {
      return End();
    }
  }

  // Find the first node whose data equals to the given data,
  // and save the prev nodes in prevs, save spans from prevs[i] to find_node
  Node* FindNodeFirstEquals(const T& data, Node** prevs, int* prev_spans) const;

  typedef std::function<bool(const T& data, const T& other)> Predicate;
  Node* Find(const T& data, Predicate&& pred) const {
    Node* p = head_;
    int level = head_->level - 1;
    while (level >= 0) {
      Node* next = p->nexts[level];
      if (nullptr != next && pred(data, next->data)) {
        // Move forward
        p = next;
        level = p->level;
      }
      --level;
    }
    return p;
  }

  // Find the first node whose data equals to the given data
  Node* FindNodeFirstEquals(const T& data) const {
    Node* p = Find(data, [this](const T& data, const T& other) -> bool {
      return Greater(data, other);
    });
    return p->nexts[0];
  }

  // Find the first node whose data is greater than the given data
  Node* FindNodeFirstGreater(const T& data) const {
    Node* p = Find(data, [this](const T& data, const T& other) -> bool {
      return GreaterOrEquals(data, other);
    });
    return p->nexts[0];
  }

  // Find the last node whose data is less than the given data
  Node* FindNodeLastLess(const T& data) const {
    Node* p = Find(data, [this](const T& data, const T& other) -> bool {
      return Greater(data, other);
    });
    return p;
  }

  bool Equals(const T& data, const T& other) const {
    return !compare_(data, other) && !compare_(other, data);
  }

  bool Less(const T& data, const T& other) const {
    return compare_(data, other);
  }

  bool Greater(const T& data, const T& other) const {
    return !Less(data, other) && !Equals(data, other);
  }

  bool GreaterOrEquals(const T& data, const T& other) const {
    return Greater(data, other) || Equals(data, other);
  }

  Node* const head_;
  Comparator const compare_;
  int size_;
};

template <typename T, typename Comparator>
typename SkipList<T, Comparator>::Node*
SkipList<T, Comparator>::FindNodeFirstEquals(const T& data, Node** prevs,
                                             int* prev_spans) const {
  Node* p = head_;
  int level = head_->level - 1;
  while (level >= 0) {
    Node* next = p->nexts[level];
    if (nullptr != next && Greater(data, next->data)) {
      // Move forward
      int span = p->spans[level];
      p = next;
      level = p->level;
      // Update prev nodes' spans
      for (int l = level; l < kMaxLevel; ++l) {
        prev_spans[l] += span;
      }
      memset(&prev_spans[0], 0, sizeof(prev_spans[0]) * level);
    } else {
      // Move down
      prevs[level] = p;
    }
    --level;
  }
  return p->nexts[0];
}

template <typename T, typename Comparator>
const T& SkipList<T, Comparator>::At(const int index) const {
  assert(index >= 0 && index < size_);

  Node* p = head_;
  int span = 0;
  int target_span = index + 1;
  int level = p->level - 1;
  for (;;) {
    Node* next = p->nexts[level];
    if (nullptr != next && target_span >= (span + p->spans[level])) {
      // Move forward
      span += p->spans[level];
      p = next;
      if (span == target_span) {
        return p->data;
      }
      level = p->level;
    }
    --level;
    assert(level >= 0);
  }
}

template <typename T, typename Comparator>
int SkipList<T, Comparator>::IndexOf(const T& data) const {
  Node* p = head_;
  int level = p->level - 1;
  int span = 0;
  while (level >= 0) {
    Node* next = p->nexts[level];
    if (nullptr != next && Greater(data, next->data)) {
      // Move forward
      span += p->spans[level];
      p = next;
      level = p->level;
    }
    --level;
  }

  Node* next = p->nexts[0];
  if (nullptr == next || !Equals(data, next->data)) {
    return -1;
  }
  return span;
}

template <typename T, typename Comparator>
bool SkipList<T, Comparator>::InternalErase(Iterator& erase_it) {
  Iterator end = End();
  if (erase_it == end) {
    return false;
  }

  Node* const erase_node = erase_it.get_node();
  Node* prevs[kMaxLevel];
  int prev_spans[kMaxLevel];
  memset(&prev_spans[0], 0, sizeof(prev_spans[0]) * kMaxLevel);

  Iterator begin = Find(erase_node->data, prevs, prev_spans);
  if (begin == end) {
    erase_it = end;
    return false;
  }

  Node* p = begin.get_node();
  int level = p->level;
  // Make sure the address of p equals to erase_node
  while (nullptr != p && p != erase_node) {
    prevs[level] = p;
    --level;
    if (level == 0) {
      p = p->nexts[0];
      level = p->level;
      // Update prevs'spans
      for (int l = level; l < kMaxLevel; ++l) {
        ++prev_spans[l];
      }
      memset(&prev_spans[0], 0, sizeof(prev_spans[0]) * level);
    }
  }

  // Invalid iterator(delete twice)
  if (p != erase_node) {
    erase_it = end;
    return false;
  }

  int erase_level = erase_node->level;
  for (int l = 0; l < erase_level; ++l) {
    // Update prevs' nexts
    prevs[l]->nexts[l] = erase_node->nexts[l];
    // Update prevs' spans and nexts' spans
    prevs[l]->spans[l] += erase_node->spans[l] - 1;
  }

  for (int l = erase_level; l < kMaxLevel; ++l) {
    // Update prevs' spans
    --prevs[l]->spans[l];
  }

  // move the iterator to next
  ++erase_it;

  delete erase_node;
  --size_;
  return true;
}

template <typename T, typename Comparator>
bool SkipList<T, Comparator>::Erase(const T& data) {
  Node* prevs[kMaxLevel];
  int prev_spans[kMaxLevel];
  memset(&prev_spans[0], 0, sizeof(prev_spans[0]) * kMaxLevel);

  Iterator it = Find(data, prevs, prev_spans);
  if (it == End()) {
    return false;
  }

  Node* erase_node = it.get_node();
  int erase_level = erase_node->level;
  for (int l = 0; l < erase_level; ++l) {
    // Update prevs' nexts
    prevs[l]->nexts[l] = erase_node->nexts[l];
    // Update prevs' spans and nexts' spans
    prevs[l]->spans[l] += erase_node->spans[l] - 1;
  }

  for (int l = erase_level; l < kMaxLevel; ++l) {
    // Update prevs' spans
    --prevs[l]->spans[l];
  }

  delete erase_node;
  --size_;
  return true;
}

template <typename T, typename Comparator>
struct SkipList<T, Comparator>::Node {
  Node(const int level_)
      : level(level_), nexts(new Node*[level]), spans(new int[level]) {}
  Node(const int level_, const T& data_)
      : data(data_),
        level(level_),
        nexts(new Node*[level]),
        spans(new int[level]) {}
  Node(const int level_, T&& data_)
      : data(std::move(data_)),
        level(level_),
        nexts(new Node*[level]),
        spans(new int[level]) {}
  ~Node() {
    delete[] nexts;
    delete[] spans;
  }

  T data;
  const int level;
  Node** nexts;
  int* spans;
};

template <typename T, typename Comparator>
class SkipList<T, Comparator>::Iterator {
 private:
  Iterator(Node* node) : node_(node) {}
  Node* get_node() const { return node_; };
  void AssertValid() { assert(nullptr != node_); }

  friend SkipList;

 public:
  ~Iterator() {}
  Iterator(const Iterator& other) { node_ = other.node_; }
  Iterator(Iterator&& other) { node_ = other.node_; }
  Iterator& operator=(const Iterator& other) {
    node_ = other.node_;
    return *this;
  }
  Iterator& operator=(Iterator&& other) { return *this = other; }

  // Compare nodes to determine if iterators are strictly equal
  bool operator==(const Iterator& other) const { return node_ == other.node_; }
  bool operator!=(const Iterator& other) const { return node_ != other.node_; }

  // Move the iterator to next
  // Prefix Increment Operator
  Iterator& operator++() {
    AssertValid();
    node_ = node_->nexts[0];
    return *this;
  }

  // Move the iterator to next
  // Postfix Increment Operator
  Iterator operator++(int) {
    AssertValid();
    Node* temp = node_;
    node_ = node_->nexts[0];
    return Iterator(temp);
  }

  // Get the reference of data
  const T& operator*() {
    AssertValid();
    return node_->data;
  }

  // Get the reference of data
  const T* operator->() {
    AssertValid();
    return &node_->data;
  }

 private:
  Node* node_;
};
#endif  // SKIPLIST_H