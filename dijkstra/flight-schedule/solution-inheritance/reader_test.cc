// reader_test.cc
//
// Copyright 2013 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)


#include <cstdlib>
#include <iostream>
#include <string>

#include "flight.h"
#include "flight_reader.h"
#include "graph.h"

using namespace std;


int main(int argc, char* argv[]) {
  string origin_name = (argc >= 2) ? argv[1] : "";
  string destination_name = (argc >= 3) ? argv[2] : "";
  Vertex* origin = NULL;
  Vertex* destination = NULL;
  Graph* g = read_flights(origin_name, destination_name, &origin, &destination);
  if (origin) {
    cout << "found origin airport: " << origin->repr() << endl;
  } else {
    cout << "no origin airport: <" << origin_name << ">" << endl;
  }
  if (destination) {
    cout << "found destination airport: " << destination->repr() << endl;
  } else {
    cout << "no destination airport: <" << destination_name << ">" << endl;
  }
  g->dump(cout);
  return 0;
}
