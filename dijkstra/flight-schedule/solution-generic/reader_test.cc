// reader_test.cc
//
// Copyright 2013 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)


#include <iostream>

#include "flight_info.h"


int main() {
  Graph<AirportInfo, FlightInfo>* g;
  g = read_graph(std::cin);
  g->dump();
  return 0;
}
