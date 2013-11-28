// priority.h
//
// PriorityQueue: binary heap priority queue
//
// Copyright 2013 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)

#if !defined(CSS343_PRIORITY_H)
#define CSS343_PRIORITY_H

#include <cassert>
#include <cstdlib>
#include <vector>

// Base class for priority queue entries.  Users of PriorityQueue may
// inherit from Priority to get the required fields and methods.  The
// purpose of this class is to ensure that the entry has a field and
// methods to support the decrease key (reduce) operation.
class Priority {
public:
  Priority() : priority_(-1) {}
  unsigned int get_priority() {return priority_;}
  void set_priority(unsigned int priority) {priority_ = priority;}
private:
  unsigned int priority_;
};


// This implementation stores pointers to Things (reference
// semantics).
//
// operator<  is used to compare Thing references.
//
// The Thing class must have methods unsigned int get_priority() and
// void set_priority(unsigned int).  These hold indices into the
// priority queue to simplify implementation of the reduce
// (decrease_key) method.
template<typename Thing>
class PriorityQueue {
public:
  PriorityQueue() {}

  // Add a new thing to the priority queue.
  void push_back(Thing* thing);

  // Decrease (raise?) the priority of a given Thing.
  void reduce(Thing* thing);

  // Remove and return the lowest (highest?) priority Thing.  This
  // differs from std::priority_queue which has separate functions.
  // Returns NULL if the queue is empty.
  Thing* pop();

  unsigned size() {return data_.size();}
  bool empty() {return data_.empty();}

  std::vector<Thing*>* debug_getdata() {return &data_;}

private:
  static unsigned int root() {return 0;}
  static bool is_root(unsigned int n) {return n == root();}
  static unsigned int parent(unsigned int n) {return (n + 1) / 2 - 1;}
  static unsigned int left(unsigned int n) {return (n + 1) * 2 - 1;}
  static unsigned int right(unsigned int n) {return left(n) + 1;}
  void swap(unsigned int n1, unsigned int n2);
  void sift_up(unsigned int n);
  void sift_down(unsigned int n);

  std::vector<Thing*> data_;
};


template<typename Thing>
void PriorityQueue<Thing>::swap(unsigned int n1, unsigned int n2) {
  Thing* tmp = data_[n1];
  data_[n1] = data_[n2];
  data_[n2] = tmp;
  data_[n1]->set_priority(n1);
  data_[n2]->set_priority(n2);
}


#if defined(RECURSIVE_IMPL)
template<typename Thing>
void PriorityQueue<Thing>::sift_up(unsigned int n) {
  if (is_root(n)) {
    return;
  }
  if (*data_[n] < *data_[parent(n)]) {
    swap(parent(n), n);
    sift_up(parent(n));
  }
}

template<typename Thing>
void PriorityQueue<Thing>::sift_down(unsigned int n) {
  if (right(n) < data_.size() && *data_[right(n)] < *data_[n] && *data_[right(n)] < *data_[left(n)]) {
    swap(n, right(n));
    sift_down(right(n));
  } else if (left(n) < data_.size() && *data_[left(n)] < *data_[n]) {
    swap(n, left(n));
    sift_down(left(n));
  }
}

#else
// Nonrecursive implementation: tail recursion is just a loop.

template<typename Thing>
void PriorityQueue<Thing>::sift_up(unsigned int n) {
  while (!is_root(n) && *data_[n] < *data_[parent(n)]) {
      swap(parent(n), n);
      n = parent(n);
  }
}

template<typename Thing>
void PriorityQueue<Thing>::sift_down(unsigned int n) {
  while (true) {
    if (right(n) < data_.size() && *data_[right(n)] < *data_[n] && *data_[right(n)] < *data_[left(n)]) {
      swap(n, right(n));
      n = right(n);
    } else if (left(n) < data_.size() && *data_[left(n)] < *data_[n]) {
      swap(n, left(n));
      n = left(n);
    } else {
      break;
    }
  }
}
#endif


template<typename Thing>
void PriorityQueue<Thing>::push_back(Thing* thing) {
  unsigned int n = data_.size();
  thing->set_priority(n);
  data_.push_back(thing);
  sift_up(n);
}


template<typename Thing>
void PriorityQueue<Thing>::reduce(Thing* thing) {
  unsigned int current_priority = thing->get_priority();
  assert(data_[current_priority] == thing);
  sift_up(current_priority);
}


template<typename Thing>
Thing* PriorityQueue<Thing>::pop() {
  if (data_.size() == 0) {
    return NULL;
  }
  Thing* min = data_[root()];
  data_[root()] = *data_.rbegin();
  data_[root()]->set_priority(root());
  data_.pop_back();
  sift_down(root());
  return min;
}


#endif
