// find_itinerary.cc
//
// See http://courses.washington.edu/css343/bernstein/2013-q4/assignments/assignment-4.html
//
// Copyright 2013 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)

#include <cassert>
#include <cstdlib>
#include <iostream>

#include "flight.h"
#include "flight_reader.h"
#include "graph.h"

using namespace std;


// Callback for generic path-walk function.
void PrintFlight(Edge* edge) {
  cout << edge->str() << endl;
}


int main(int argc, char* argv[]) {
  if (argc != 3) {
    cerr << "usage: " << argv[0] << " origin destination < data" << endl;
    return EXIT_FAILURE;
  }

  string origin = argv[1];
  string destination = argv[2];
  Vertex* from = NULL;
  Vertex* to = NULL;
  Graph* flight_graph = read_flights(origin, destination, &from, &to);
  if (!from) {
    cerr << "origin airport not found: " << origin << endl;
    return EXIT_FAILURE;
  }
  if (!to) {
    cerr << "destination airport not found: " << destination << endl;
    return EXIT_FAILURE;
  }

  Vertex* destination_found = flight_graph->find_minimum_path(from, to);
  if (!destination_found) {
    cerr << "No flight found from " << origin << " to " << destination << endl;
    exit(EXIT_FAILURE);
  }
  destination_found->walk_minimum_path(NULL, PrintFlight);
  return EXIT_SUCCESS;
}
