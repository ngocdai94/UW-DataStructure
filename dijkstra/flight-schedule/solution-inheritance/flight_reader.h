// flight_reader.h
//
// See: http://courses.washington.edu/css343/bernstein/2013-q4/assignments/assignment-3.html
//
// Copyright 2013 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)

#if !defined(CSS343_FLIGHT_READER_H)
#define CSS343_FLIGHT_READER_H

#include <iostream>
#include <string>

#include "graph.h"
#include "flight.h"


// Read in flight data graph, setting origin and final_destination
// vertices if found.
Graph* read_flights(const std::string& origin_name, const std::string& destination_name,
                    Vertex** origin, Vertex** final_destination,
                    std::istream& in=std::cin);


#endif
