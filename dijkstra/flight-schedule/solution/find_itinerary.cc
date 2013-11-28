// find_itinerary.cc
//
// See http://courses.washington.edu/css343/bernstein/2013-q4/assignments/assignment-3.html
//
// Copyright 2013 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)

#include <cassert>
#include <cstdlib>
#include <iostream>

#include "graph.h"

using namespace std;


int main(int argc, char* argv[]) {
  if (argc != 3) {
    cerr << "usage: " << argv[0] << " origin destination" << endl;
    return EXIT_FAILURE;
  }

  string origin = argv[1];
  string destination = argv[2];

  Graph* flight_data = graph_reader(cin);
  assert(flight_data);

  Vertex* destination_vertex = flight_data->find_itinerary(origin, destination);
  if (!destination_vertex) {
    cout << "No flight found from " << origin << " to " << destination << endl;
  } else {
    flight_data->print_itinerary(destination_vertex);
  }

  return EXIT_SUCCESS;
}
