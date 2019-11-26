#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <algorithm>
#include <atomic>
#include <cassert>
#include <cstring>
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

  SkipList() : head_(new Node(kMaxLevel)), compare_() {
    memset(&head_->nexts[0], 0, sizeof(head_->nexts[0]) * kMaxLevel);
    memset(&head_->spans[0], 0, sizeof(head_->spans[0]) * kMaxLevel);
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
    Node* prevs[kMaxLevel];
    return Find(data, prevs);
  }

  Iterator Find(const T& data, Node** prevs) const {
    Node* find_node = FindFirstGreaterOrEquals(data, prevs);
    if (nullptr != find_node && Equals(data, find_node->data)) {
      return Iterator(find_node);
    } else {
      return End();
    }
  }

  // Delete the node whose address is equals to
  // the address of node of the given iterator.
  // If success return true and change erase_it to ++erase_it.
  bool Erase(Iterator&& erase_it) { return InternalErase(erase_it); }

  // Delete the node whose address is equals to
  // the address of node of the given iterator.
  // If success return true and change erase_it to ++erase_it.
  bool Erase(Iterator& erase_it) { return InternalErase(erase_it); }

  Iterator begin() const { return Iterator(head_->nexts[0]); }
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
    FindFirstGreaterOrEquals(new_node->data, prevs);

    for (int i = 0; i < new_node->level; ++i) {
      // Update prevs' nexts and new_node's nexts
      new_node->nexts[i] = prevs[i]->nexts[i];
      prevs[i]->nexts[i] = new_node;
    }
  }

  // Find the first node whose data is greater than or equal to the given data
  Node* FindFirstGreaterOrEquals(const T& data, Node** prevs) const {
    Node* p = head_;
    int level = head_->level - 1;
    while (level >= 0) {
      Node* next = p->nexts[level];
      if (nullptr != next && Greater(data, next->data)) {
        p = next;
        level = p->level;
      } else {
        prevs[level] = p;
      }
      --level;
    }
    return p->nexts[0];
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

  Node* const head_;
  Comparator const compare_;
};

template <typename T, typename Comparator>
bool SkipList<T, Comparator>::InternalErase(Iterator& erase_it) {
  Iterator end = End();
  if (erase_it == end) {
    return false;
  }

  Node* const erase_node = erase_it.get_node();
  Node* prevs[kMaxLevel];
  Iterator begin = Find(erase_node->data, prevs);
  if (begin == end) {
    erase_it = end;
    return false;
  }

  Node* p = begin.get_node();
  int level = p->level;
  while (nullptr != p && p != erase_node) {
    prevs[level] = p;
    --level;
    if (level == 0) {
      p = p->nexts[0];
      level = p->level;
    }
  }

  // Illegal param
  assert(p == erase_node);

  for (int i = 0; i < erase_node->level; ++i) {
    // Update prevs' nexts
    prevs[i]->nexts[i] = erase_node->nexts[i];
  }

  // move the iterator to next
  ++erase_it;
  delete erase_node;
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
  Iterator& operator++() {
    node_ = node_->nexts[0];
    return *this;
  }
  // Get the reference of data
  const T& operator*() { return node_->data; }

 private:
  Node* node_;
};
#endif  // SKIPLIST_H