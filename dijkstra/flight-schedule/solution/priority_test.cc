// priority_test.cc
//
// Copyright 2013 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)

#include "priority.h"

#include <cassert>
#include <iostream>

using namespace std;


class PriorityString {
public:
  PriorityString(const string& initial_value) : value(initial_value), priority(0) {
  }

  void set_priority(int n) {priority = n;}

  string value;
  int priority;
};


class StringCmp {
public:
  int operator() (const PriorityString* lhs, const PriorityString* rhs) {
    if (lhs->value < rhs->value) {
      return -1;
    } else if (lhs->value > rhs->value) {
      return 1;
    } else return 0;
  }
};


int main() {
  StringCmp cmp;
  PriorityQueue<PriorityString, StringCmp> pq(cmp);
  string word;

  while (cin >> word) {
    pq.push_back(new PriorityString(word));
  }

  while (!pq.empty()) {
    PriorityString *pword = pq.pop();
    assert(pword);
    cout << pword->value << endl;
    delete pword;
  }

  return 0;
}
