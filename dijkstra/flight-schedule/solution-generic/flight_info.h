// flight_info.h
//
// Copyright 2013 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)

#if !defined(CSS343_FLIGHT_INFO_H)
#define CSS343_FLIGHT_INFO_H

#include <iostream>
#include <istream>
#include <string>
#include <vector>

#include "graph.h"
#include "../time/time.h"


class AirportInfo;
class FlightInfo;

typedef Graph<AirportInfo, FlightInfo> AirportGraph;


// Information about all flights between a pair of airports (single
// edge in the graph): essentially, just a list of Flight objects.
class FlightInfo {
public:
  // Information about a single flight: just the departure and arrival
  // times.  Origin and destination airports are determined by the
  // Edge to which the flight(s) belong.
  class Flight {
  public:
    Flight(const TimePoint& dep_time, const TimePoint& arr_time)
      : dep_time_(dep_time), arr_time_(arr_time) {
    }

    const TimePoint& departure_time() const {
      return dep_time_;
    }

    const TimePoint& arrival_time() const {
      return arr_time_;
    }

    void dump(std::ostream& out=std::cerr, int depth = 0) const;

  private:
    const TimePoint dep_time_;
    const TimePoint arr_time_;

    // DISALLOW_COPY_AND_ASSIGN (do not implement)
    Flight(const Flight&);
    void operator=(const Flight&);
  };

  typedef std::vector<Flight*> Flights;

  FlightInfo() {}

  void add_flight(Flight* flight) {flights_.push_back(flight);}

  // Given an earliest departure time and current best-known arrival
  // time, return the flight with the earliest arrival time earlier
  // than current_arrival if it departs after the earliest
  // departure.  Return NULL otherwise.
  Flight* better(TimePoint earliest_departure, TimePoint current_arrival);

  // String representation of object (for debugging).
  std::string repr() {return "flight";}

  void dump(std::ostream& out = std::cerr);

private:
  Flights flights_;

  // DISALLOW_COPY_AND_ASSIGN (do not implement)
  FlightInfo(const FlightInfo&);
  void operator=(const FlightInfo&);
};


// Information about airports: cost/cost-update, name and last leg of
// best-known flight to get here, used by generic Dijkstra's graph
// algorithm.
class AirportInfo {
public:
  AirportInfo(const std::string& name)
    : name_(name), parent_flight_(NULL), is_origin_(false) {
  }

  void initialize_cost(AirportGraph::Vertex* origin);
  bool update_cost(AirportInfo* from, FlightInfo* leg);

  FlightInfo::Flight* flight() {return parent_flight_;}

  std::string repr() {return name_;}

  bool is_named(const std::string& name) {
    return name == name_;
  }

  // Compare arrival times to get to airport.
  bool operator<(const AirportInfo& other) const {
    return best_arrival_ < other.best_arrival_;
  }

private:
  std::string name_;
  TimePoint best_arrival_;
  FlightInfo::Flight* parent_flight_;
  bool is_origin_;

  // DISALLOW_COPY_AND_ASSIGN (do not implement)
  AirportInfo(const AirportInfo&);
  void operator=(const AirportInfo&);
};


class VertexByName {
public:
  VertexByName(const std::string& name) : name_(name) {}
  bool operator()(AirportInfo* info) {
    return info->is_named(name_);
  }
private:
  std::string name_;
};



class Printer {
public:
  void operator()(AirportInfo* from_info, AirportInfo* to_info, FlightInfo* flight_info);
};


AirportGraph* read_graph(std::istream& in);


#endif
