// flight.cc
//
// Copyright 2013 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)


#include "flight.h"

using namespace std;


void Flight::dump(ostream& out, int depth) const {
    string indent = string(4 * depth, ' ' );
    out << indent << "Flight:" << endl;
    out << indent << "    departs: ";
    dep_time_.dump(out, 0);
    out << indent << "    arrives: ";
    arr_time_.dump(out, 0);
    out << indent << "    time enroute: ";
    TimeDiff(dep_time_, arr_time_).dump(out, 0);
  }
