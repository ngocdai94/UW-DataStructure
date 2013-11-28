// flight.h
//
// See: http://courses.washington.edu/css343/bernstein/2013-q4/assignments/assignment-3.html
//
// Copyright 2013 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)

#if !defined(CSS343_FLIGHT_H)
#define CSS343_FLIGHT_H

#include <cassert>
#include <iostream>
#include <istream>
#include <string>
#include <vector>

#include "graph.h"
#include "../time/time.h"


class Airport : public Vertex {
public:
  Airport(std::string name) : name_(name) {}

  virtual void initialize_cost(Vertex* origin);
  virtual bool update_cost(Edge* edge);
  virtual bool operator<(const Vertex& other) {
    const Airport* other_airport = dynamic_cast<const Airport*>(&other);
    assert(other_airport);
    return tentative_best_arrival_ < other_airport->tentative_best_arrival_;
  }

  const std::string& name() const {return name_;}

  // str() gives a string representation; repr() gives a string
  // representation for debugging.
  virtual std::string str() const {return name_;};
  virtual std::string repr() const;

private:
  const std::string name_;
  TimePoint tentative_best_arrival_;

  // DISALLOW_COPY_AND_ASSIGN (do not implement)
  Airport(const Airport&);
  void operator=(const Airport&);
};


class Flight : public Edge {
public:
  Flight(Airport* departure, Airport* destination,
         const TimePoint& dep_time, const TimePoint& arr_time)
    : Edge(departure, destination), dep_time_(dep_time), arr_time_(arr_time) {
  }

  const TimePoint& departure_time() {return dep_time_;}
  const TimePoint& arrival_time() {return arr_time_;}

  virtual std::string str() const;
  virtual std::string repr() const;

private:
  const TimePoint dep_time_;
  const TimePoint arr_time_;

  // DISALLOW_COPY_AND_ASSIGN (do not implement)
  Flight(const Flight&);
  void operator=(const Flight&);
};


#endif
