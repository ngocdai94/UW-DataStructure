// graph.cc
//
// Copyright 2013 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)


#include "graph.h"

#include <cassert>
#include <iostream>

using namespace std;


// Minimum layover: 1 hour.
TimeDiff Vertex::minimum_layover_time_ = TimeDiff(60);


// Function object to compare nodes by best known arrival time, for
// use with the priority queue.  Returns negative result for
// less-than, positive for greater-than, and zero for equality.
class CompareVertices {
public:
  int operator()(Vertex* v1, Vertex* v2) {
    return time_compare(v1->best_arrival_time_, v2->best_arrival_time_);
  }
};


// Iterate through the flights to find the best flight leaving later
// than the earliest departure time.  Return the flight only if it's
// better than the incoming current_best_arrival time (of the
// destination airport).
Flight* Edge::better_flight(const TimePoint& earliest_departure, TimePoint current_best_arrival)  const {
  Flight* better_flight = NULL;
  for (Flights::const_iterator flight_it = flights_.begin(); flight_it != flights_.end(); ++flight_it) {
    if ((time_compare((*flight_it)->departure_time(), earliest_departure) >= 0) &&
        (time_compare((*flight_it)->arrival_time(), current_best_arrival) < 0)) {
      better_flight = *flight_it;
      current_best_arrival = (*flight_it)->arrival_time();
    }
  }
  if (better_flight) {
    cerr << "      better flight to " << dest_->airport_id()
         << " leaving " << better_flight->departure_time()
         << ", arriving " << better_flight->arrival_time()
         << endl;
  }
  return better_flight;
}


void Edge::dump(ostream& out) const {
  out << "    Travel to " << dest_->airport_id() << " (" << dest_ << ")" << endl;
  for (Flights::const_iterator it = flights_.begin(); it !=flights_.end(); ++it) {
    (*it)->dump(out, 2);
  }
}


void Vertex::initialize_search(const string& origin) {
  if (origin == airport_) {
    best_arrival_time_ = TimePoint(0);
  } else {
    best_arrival_time_ = TimePoint::max_time;
  }
  parent_ = NULL;
  parent_flight_ = NULL;
  priority_ = -1;
}


void Vertex::update_neighbors(PriorityQueue<Vertex, CompareVertices>* priority) {
  TimePoint earliest_departure = best_arrival_time_ + minimum_layover_time_;
  cerr << "   updating neigbors of " << airport_
       << " (earliest departure time: " << earliest_departure << ")" << endl;
  for (Edges::iterator edge_it = edges_.begin(); edge_it != edges_.end(); ++edge_it) {
    Vertex* leg = (*edge_it)->destination();
    if (time_compare(leg->best_arrival_time_, earliest_departure) <= 0) {
      // Current best arrival time for the destination is better than our earliest departure time
    } else {
      Flight* better_flight = (*edge_it)->better_flight(earliest_departure, leg->best_arrival_time_);
      if (better_flight) {
        cerr << "         updating arrival time to " << leg->airport_
             << " departing " << better_flight->departure_time()
             << ", arriving " << better_flight->arrival_time()
             << endl;
        leg->parent_ = this;
        leg->parent_flight_ = better_flight;
        leg->best_arrival_time_ = better_flight->arrival_time();
        priority->reduce(leg);
      }
    }
  }
}


// Dijkstra's algorithm.
Vertex* Graph::find_itinerary(const string& origin, const string& destination) {
  CompareVertices cmp;
  PriorityQueue<Vertex, CompareVertices> priority(cmp);

  bool found_origin = false;
  bool found_destination = false;

  // Initialization
  for (Vertices::iterator it = vertices_.begin(); it != vertices_.end(); ++it) {
    if ((*it)->airport_id() == origin) {
      found_origin = true;
    }
    if ((*it)->airport_id() == destination) {
      found_destination = true;
    }
    (*it)->initialize_search(origin);
    priority.push_back(*it);
  }

  if (!found_origin || !found_destination) {
    if (!found_origin) {
      cerr << "Missing origin airport " << origin << endl;
    }
    if (!found_destination) {
      cerr << "Missing destination airport " << destination << endl;
    }
    return NULL;
  }

  // Dijstra's greedy algorithm: keep selecting the next cheapest vertex
  // and updating its children until the goal is the the next cheapest
  // node.
  while (1) {
    assert(!priority.empty());
    Vertex* next_vertex = priority.pop();
    cerr << "find_itinerary(): next_vertex: " << next_vertex->airport_id()
         << " arrival time: " << next_vertex->best_arrival_time()
         << endl;
    if (next_vertex->airport_id() == destination) {
      return next_vertex->has_valid_route() ? next_vertex : NULL;
    }
    next_vertex->update_neighbors(&priority);
  }
}


// Recursively print each leg of the itinerary.  Since we start from
// the destination and work backwards, we need to recurse first and
// then print.
void Vertex::print_itinerary() const {
  if (parent_) {
    parent_->print_itinerary();
    cout << parent_->airport_ << "    "
         << parent_flight_->departure_time() << "   "
         << airport_ << "    "
         << parent_flight_->arrival_time() << endl;
  }
}


void Vertex::dump(ostream& out) const {
  out << airport_ << " (" << this << ")" << endl;
  for (Edges::const_iterator it = edges_.begin(); it != edges_.end(); ++it) {
    (*it)->dump(out);
  }
}


void Graph::print_itinerary(const Vertex* destination) const {
  destination->print_itinerary();
}


void Graph::dump(ostream& out) const {
  for (Vertices::const_iterator it = vertices_.begin() ; it != vertices_.end(); ++it) {
    (*it)->dump(out);
  }
}
