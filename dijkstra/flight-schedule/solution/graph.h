// graph.h
//
// Graph of flight data.
//
// Copyright 2013 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)

#if !defined(CSS343_GRAPH_H)
#define CSS343_GRAPH_H

#include <istream>
#include <string>
#include <vector>

#include "flight.h"
#include "priority.h"
#include "../time/time.h"


class CompareVertices;
class Graph;
class Vertex;
class Edge;


typedef std::vector<Vertex*> Vertices;
typedef std::vector<Edge*> Edges;
typedef std::vector<Flight*> Flights;


class Edge {
public:
  Edge(Vertex* dest) : dest_(dest) {}

  // Given an edge from a departure to destination airport, return a
  // flight that departs no earlier than the earliest departure time
  // (previous leg's arrival time plus mandatory minimum layover
  // time), provided it is better than the destination's current best
  // arrival time.  Return NULL otherwise.
  Flight* better_flight(const TimePoint& earliest_departure, TimePoint current_best_arrival) const;

  Vertex* destination() const {
    return dest_;
  }

  void dump(std::ostream& out=std::cerr) const;

  friend Graph* graph_reader(std::istream& in);

private:
  Vertex* dest_;
  Flights flights_;

  // DISALLOW_COPY_AND_ASSIGN (do not implement)
  Edge(const Edge&);
  void operator=(const Edge&);
};


class Vertex {
public:
  Vertex(const std::string airport) : airport_(airport) {}

  // Intialization for Dijkstra's shortest-path algorithm.  The
  // departure airport has minimum arrival time; all other airports
  // have infinite arival time, and NULL back-pointer.
  void initialize_search(const std::string& origin);

  // Check all destination airports for the current departure
  // airport.  If better_flight is found, update the destination
  // airport's current_best_arrival_time.  Updates will affect their
  // position in the priority queue.
  void update_neighbors(PriorityQueue<Vertex, CompareVertices>* priority);

  bool has_valid_route() {
    return !(best_arrival_time_ == TimePoint::max_time);
  }

  // Work backwards from destination aiport to departure airport (of a
  // possibly multi-legged flight), reversing the order to print the
  // flight data correctly.
  void print_itinerary() const;

  // Return the Vertex's position in the priority queue.  This is an
  // index into the queue, not a priority value, but it is is set and
  // used only by the PriorityQueue class and otherwise treated as an
  // opaque type.
  int get_priority() const {
    return priority_;
  }

  // Used by the PriorityQueue class to manage the decrease-key
  // operation.
  void set_priority(int priority) {
    priority_ = priority;
  }

  const std::string& airport_id() const {
    return airport_;
  }

  // best_arrival_time is the currently-known best arrival time to
  // this airport or TimePoint::max_time if unknown or unreachable.
  TimePoint& best_arrival_time() {
    return best_arrival_time_;
  }

  void dump(std::ostream& out=std::cerr) const;

private:
  // Comparison operation for use by PriorityQueue for finding the
  // shortest route.
  friend class CompareVertices;

  friend Graph* graph_reader(std::istream& in);

  static TimeDiff minimum_layover_time_;

  const std::string airport_;
  Edges edges_;

  // Additional fields for finding the best routing (Dijkstra's
  // algorithm).  parent_ is the predecessor airport, forming a linked
  // list from the destination to the departure airport.  To process
  // in correct order, the list must be reversed.
  Vertex* parent_;
  Flight* parent_flight_;
  TimePoint best_arrival_time_;
  int priority_;

  // DISALLOW_COPY_AND_ASSIGN (do not implement)
  Vertex(const Vertex&);
  void operator=(const Vertex&);
};


class Graph {
public:
  Graph() {}

  // Returns pointer to destination vertex or NULL if there is no valid
  // route.
  Vertex* find_itinerary(const std::string& origin, const std::string& destination);

  void print_itinerary(const Vertex* destination) const;

  void dump(std::ostream& out=std::cerr) const;

  friend Graph* graph_reader(std::istream& in);

private:
  Vertices vertices_;

  // DISALLOW_COPY_AND_ASSIGN (do not implement)
  Graph(const Graph&);
  void operator=(const Graph&);
};


Graph* graph_reader(std::istream& in);


#endif
