// find_itinerary.cc
//
// See http://courses.washington.edu/css343/bernstein/2013-q4/assignments/assignment-4.html
//
// Copyright 2013 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)

#include <cstdlib>
#include <cstdlib>
#include <iostream>

#include "flight_info.h"
#include "graph.h"

using namespace std;


AirportGraph::Vertex* find_airport_or_die(AirportGraph* g, const string& name, const char* kind) {
  VertexByName predicate = VertexByName(name);
  AirportGraph::Vertex* v = g->find_vertex(predicate);
  if (!v) {
    cerr << "error: " << kind << " airport " << name << " not found" << endl;
    exit(EXIT_FAILURE);
  }
  return v;
}


int main(int argc, char* argv[]) {
  if (argc != 3) {
    cerr << "usage: " << argv[0] << " origin destination" << endl;
    return EXIT_FAILURE;
  }
  string origin = argv[1];
  string destination = argv[2];

  AirportGraph* flight_graph = read_graph(cin);
  AirportGraph::Vertex* from = find_airport_or_die(flight_graph, origin, "origin");
  AirportGraph::Vertex* to = find_airport_or_die(flight_graph, destination, "destination");

  AirportGraph::Vertex* destination_found = flight_graph->find_minimum_path(from, to);
  if (!destination_found) {
    cout << "No flight found from " << origin << " to " << destination << endl;
    exit(EXIT_FAILURE);
  }

  Printer do_itinerary;
  flight_graph->walk_minimum_path(destination_found, do_itinerary);
  return EXIT_SUCCESS;
}
