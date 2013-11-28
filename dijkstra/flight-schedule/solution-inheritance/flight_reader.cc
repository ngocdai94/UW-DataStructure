// flight_reader.cc
//
// Copyright 2013 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)


#include "flight_reader.h"

#include <cstdlib>

#include "../time/time.h"

using namespace std;


static Airport* read_airport(istream& in, Graph* g, map<string, Airport*>* airports) {
  string name;
  in >> name;
  if (in.eof()) {
    return NULL;
  }
  if (!in.good()) {
    cerr << "flight reader: I/O error\n" << endl;
    exit(EXIT_FAILURE);
  }
  Airport*& airport = (*airports)[name];
  if (!airport) {
    airport = new Airport(name);
    assert(airport);
    g->add_vertex(airport);
  }
  return airport;
}


static TimePoint read_time(istream& in) {
  unsigned int raw_time;
  in >> raw_time;
  if (!in.good()) {
    cerr << "graph reader: error reading time value\n" << endl;
    exit(EXIT_FAILURE);
  }
  return TimePoint(raw_time);
}


Graph* read_flights(const string& origin_name, const string& destination_name,
                    Vertex** origin, Vertex** final_destination,
                    istream& in) {
  Graph* g = new Graph;
  assert(g);

  // Flags find_origin & find_destination determine whether to look
  // for origin and desination vertices.  Stop looking after they're
  // found.
  bool find_origin = (origin != NULL);
  bool find_destination = (final_destination != NULL);

  map<string, Airport*> airports;
  while (true) {
    Airport* departure = read_airport(in, g, &airports);
    if (!departure) {
      return g;
    }
    if (find_origin && departure->name() == origin_name) {
      find_origin = false;
      *origin = departure;
    }

    TimePoint departure_time = read_time(in);

    Airport* destination = read_airport(in, g, &airports);
    if (!destination) {
      cerr << "graph reader: error reading destination airport from " << departure->repr() << endl;
      exit(EXIT_FAILURE);
    }
    if (find_destination && destination->name() == destination_name) {
      find_destination = false;
      *final_destination = destination;
    }

    TimePoint arrival_time = read_time(in);

    new Flight(departure, destination, departure_time, arrival_time);
  }
}
