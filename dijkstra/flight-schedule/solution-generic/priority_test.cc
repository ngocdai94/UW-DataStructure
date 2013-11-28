// priority_test.cc
//
// Copyright 2013 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)

#include "priority.h"

#include <cassert>
#include <iostream>

using namespace std;


class PriorityString : public Priority {
public:
  PriorityString(const string& initial_value) : value(initial_value) {}
  bool operator<(const PriorityString& other) const {return this->value < other.value;}
  const string value;
};

void dump(PriorityQueue<PriorityString>* pq) {
  for (unsigned n = 0 ; n < pq->size(); ++n) {
    cerr << n << ":  " << (*pq->debug_getdata())[n]->value << endl;
  }
}



int main() {
  PriorityQueue<PriorityString> pq;

  string word;
  while (cin >> word) {
    cerr << "**** inserting word: " << word << endl;
    pq.push_back(new PriorityString(word));
    dump(&pq);
  }

  while (!pq.empty()) {
    PriorityString *pword = pq.pop();
    assert(pword);
    cerr << "**** extracted word: " << pword->value << endl;
    dump(&pq);
    cout << pword->value << endl;
    delete pword;
  }

  return 0;
}
