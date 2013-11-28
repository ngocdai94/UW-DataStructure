// flight.cc
//
// Copyright 2013 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)


#include "flight.h"

#include <cstdlib>
#include <iostream>
#include <istream>
#include <map>
#include <sstream>

#include "graph.h"
#include "../time/time.h"


using namespace std;


// Minimum layover: 1 hour.
static const TimeDiff MINIMUM_LAYOVER = TimeDiff(60);


void Airport::initialize_cost(Vertex* origin) {
  tentative_best_arrival_ = (this == origin) ?
    TimePoint::min_time : TimePoint::max_time;
}


bool Airport::update_cost(Edge* edge) {
  // Downcasting is okay here because we're operating in the realm of
  // airports and flight rather than the domain of vertices and
  // edges.
  Flight* flight = dynamic_cast<Flight*>(edge);
  assert(flight);
  Airport* from = dynamic_cast<Airport*>(edge->from());
  assert(from);

  // Add the minimum layover time to the arrival-time-into-the-origin
  // only if the origin airport is an intermediate stopover point.
  TimePoint earliest_departure_time = from->tentative_best_arrival_;
  if (!(earliest_departure_time == TimePoint::min_time)) {
    earliest_departure_time = earliest_departure_time + MINIMUM_LAYOVER;
  }

  bool updated = false;
  if (!(flight->departure_time() < earliest_departure_time) &&
      (flight->arrival_time() < tentative_best_arrival_)) {
    cerr << "    Airport: " << repr() << ": found better flight" << endl;
    cerr << "        " << flight->repr() << endl;
    tentative_best_arrival_ = flight->arrival_time();
    cerr << "        updated arrival time: " << tentative_best_arrival_.repr() << endl;
    updated = true;
  }
  return updated;
}


string Airport::repr() const {
  stringstream s;
  s << Vertex::repr()
    << " " << name_;
  return s.str();
}


string Flight::str() const {
  stringstream s;
  s << from()->str()
    << "   " << dep_time_.repr()
    << "   " << to()->str()
    << "   " << arr_time_.repr();
  return s.str();
}


string Flight::repr() const {
  stringstream s;
  s <<  Edge::repr()
    << " departure " << dep_time_.repr()
    << " arriving " << arr_time_.repr();
  return s.str();
}
