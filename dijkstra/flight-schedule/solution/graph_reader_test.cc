// graph_reader_test.h
//
// Copyright 2013 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)

#include <iostream>

#include "graph.h"

using namespace std;

int main() {
  Graph* g = graph_reader(cin);
  g->dump();
  return 0;
}
