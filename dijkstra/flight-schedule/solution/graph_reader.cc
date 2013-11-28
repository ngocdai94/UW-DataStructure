// graph_reader.h
//
// Copyright 2013 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)


#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <utility>

#include "graph.h"
#include "../time/time.h"

using namespace std;


Graph* graph_reader(istream& in) {
  map<string, Vertex*> vertex_map;
  map<string, map<string, Edge*> > edge_map;

  Graph* g = new Graph;
  while (true) {
    string departure;
    in >> departure;
    if (in.eof()) {
      return g;
    }
    if (!in.good()) {
      cerr << "graph reader: error reading departure airport\n" << endl;
      exit(EXIT_FAILURE);
    }

    unsigned int departure_time;
    in >> departure_time;
    if (!in.good()) {
      cerr << "graph reader: error reading departure time\n" << endl;
      exit(EXIT_FAILURE);
    }
    TimePoint departure_time_point(departure_time);

    string destination;
    in >> destination;
    if (!in.good()) {
      cerr << "graph reader: error reading destination\n" << endl;
      exit(EXIT_FAILURE);
    }

    unsigned int arrival_time;
    in >> arrival_time;
    if (!in.good()) {
      cerr << "graph reader: error reading arrival time\n" << endl;
      exit(EXIT_FAILURE);
    }
    TimePoint arrival_time_point(arrival_time);

    Vertex*& departure_vertex = vertex_map[departure];
    if (departure_vertex == NULL) {
      departure_vertex = new Vertex(departure);
      g->vertices_.push_back(departure_vertex);
    }

    Vertex*& destination_vertex = vertex_map[destination];
    if (destination_vertex == NULL) {
      destination_vertex = new Vertex(destination);
      g->vertices_.push_back(destination_vertex);
    }

    Edge*& edge = edge_map[departure][destination];
    if (edge == NULL) {
      edge = new Edge(destination_vertex);
      departure_vertex->edges_.push_back(edge);
    }

    Flight* flight = new Flight(departure_time_point, arrival_time_point);
    edge->flights_.push_back(flight);
  }
}
