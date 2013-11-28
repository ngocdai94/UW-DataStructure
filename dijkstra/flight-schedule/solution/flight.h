// flight.h
//
// Class Flight represents a flight, essentially a pair of departure
// and arrival times.
//
// Copyright 2013 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)


#if !defined(CSS343_FLIGHT_H_)
#define CSS343_FLIGHT_H_

#include <iostream>
#include <ostream>
#include <string>

#include "../time/time.h"


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
};


#endif
