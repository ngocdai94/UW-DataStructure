// flight_info.cc
//
// Copyright 2013 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)


#include "flight_info.h"

#include <cstdlib>
#include <iostream>
#include <istream>
#include <map>

#include "graph.h"
#include "../time/time.h"


using namespace std;


// Minimum layover: 1 hour.
static const TimeDiff MINIMUM_LAYOVER = TimeDiff(60);


void AirportInfo::initialize_cost(AirportGraph::Vertex* origin) {
  if (this == origin->info()) {
    best_arrival_  = TimePoint(0);
    is_origin_ = true;
  } else {
    best_arrival_ = TimePoint::max_time;
    is_origin_ = false;
  }
}


// Iterate through the flights to find the best flight leaving later
// than the earliest departure time.  Return true only if it's better
// than the current best-arrival time.
bool AirportInfo::update_cost(AirportInfo* from, FlightInfo* leg) {
  TimePoint earliest_departure = from->best_arrival_;
  if (!from->is_origin_) {
    earliest_departure = earliest_departure + MINIMUM_LAYOVER;
  }
  cerr << "    AirportInfo::update_cost() from "
       << repr()
       << ": earliest departure: "
       << earliest_departure.repr()
       << endl;
  FlightInfo::Flight* better_flight = leg->better(earliest_departure, best_arrival_);
  if (better_flight) {
    parent_flight_ = better_flight;
    best_arrival_ = better_flight->arrival_time();
    cerr << "    AirportInfo::update_cost(): found better flight to " << repr()
         << " via " << from->repr()
         << " leaving " << better_flight->departure_time()
         << ", arriving " << better_flight->arrival_time()
         << endl;
  }
  return better_flight;
}


FlightInfo::Flight* FlightInfo::better(TimePoint earliest_departure, TimePoint current_arrival) {
  FlightInfo::Flight* better_flight = NULL;
  for (FlightInfo::Flights::const_iterator it = flights_.begin(); it != flights_.end(); ++it) {
    if ((time_compare((*it)->departure_time(), earliest_departure) >= 0) &&
        (time_compare((*it)->arrival_time(), current_arrival) < 0)) {
      better_flight = *it;
      current_arrival = (*it)->arrival_time();
    }
  }
  return better_flight;
}


void FlightInfo::dump(ostream& out) {
  for (Flights::const_iterator it = flights_.begin(); it != flights_.end(); ++it) {
    (*it)->dump(out, 2);
  }
}


void FlightInfo::Flight::dump(ostream& out, int depth) const {
  string indent = string(4 * depth, ' ' );
  out << indent << "Flight:" << endl;
  out << indent << "    departs: ";
  dep_time_.dump(out, 0);
  out << indent << "    arrives: ";
  arr_time_.dump(out, 0);
  out << indent << "    time enroute: ";
  TimeDiff(dep_time_, arr_time_).dump(out, 0);
}


void Printer::operator()(AirportInfo* from_info, AirportInfo* to_info, FlightInfo* flight_info) {
  cout << from_info->repr()
       << "   " << to_info->flight()->departure_time().repr()
       << "   " << to_info-> repr()
       << "   " << to_info->flight()->arrival_time().repr()
       << endl;
}


static string read_departure(istream& in) {
  string departure;
  in >> departure;
  if (in.eof()) {
    return "";
  }
  if (!in.good()) {
    cerr << "graph reader: I/O error\n" << endl;
    exit(EXIT_FAILURE);
  }
  return departure;
}


static string read_destination(istream& in) {
  string destination;
  in >> destination;
  if (!in.good()) {
    cerr << "graph reader: error reading destination\n" << endl;
    exit(EXIT_FAILURE);
  }
  return destination;
}


static unsigned int read_time(istream& in) {
  unsigned int time;
  in >> time;
  if (!in.good()) {
    cerr << "graph reader: error reading time value\n" << endl;
    exit(EXIT_FAILURE);
  }
  return time;
}


AirportGraph* read_graph(istream& in) {
  AirportGraph* g = new AirportGraph;
  map<string, AirportGraph::Vertex*> vertex_map;
  map<string, map<string, AirportGraph::Edge*> > edge_map;

  while (true) {
    string departure = read_departure(in);
    if (departure == "") {
      return g;
    }
    TimePoint departure_time_point(read_time(in));
    string destination = read_destination(in);
    TimePoint arrival_time_point(read_time(in));

    AirportGraph::Vertex*& departure_vertex = vertex_map[departure];
    if (!departure_vertex) {
      departure_vertex = g->add_vertex(new AirportInfo(departure));
    }

    AirportGraph::Vertex*& destination_vertex = vertex_map[destination];
    if (!destination_vertex) {
      destination_vertex = g->add_vertex(new AirportInfo(destination));
    }

    AirportGraph::Edge*& edge = edge_map[departure][destination];
    if (!edge) {
      edge = g->add_edge(new FlightInfo, departure_vertex, destination_vertex);
    }

    FlightInfo::Flight* flight = new FlightInfo::Flight(departure_time_point, arrival_time_point);
    edge->info()->add_flight(flight);
  }
}
