// build.h
//
// Provides Function to build graph from input file per the
// CSS 343 Assignment-4 input specification.
//
// Copyright 2013 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)

#if !defined(CSSS343_BUILD_H)
#define CSSS343_BUILD_H

#include <istream>

namespace css343 {

  // META: rather than #include "graph-draw.h", we just provide a
  // META: forward declaration.  The compiler only needs to know that
  // META: GraphDraw is a class to create a pointer-to-class parameter
  // META: for the Build() function.
  class GraphDraw;

  // META: Using namespace css343::dijkstra for this code as if this
  // META: were some piece of a larger project, say for a
  // META: 50,000-person engineering company with a codebase in the
  // META: millions of lines.
  namespace dijkstra {

    class Graph;

    // META: A lot of boilerplate just to declare one function.
    // META: Notice that the istream is a  parameter.  The application
    // META: wants to read from cin, but there's little cost
    // META: parameterizing the input file.  This allows for alternate
    // META: implementations.
    // Builds graph from input file that contains tab-separated triple
    // (from, to, cost).  gd may be null if not building graphic
    // output.
    // e.g.:
    // HOU	SEA	10
    // SEA	PDX	5
    // PDX	BLI	8
    // META: can also run tests reading from a stringstream:
    // e.g. stringstream("HOU\tSEA\t10\nSEA\tPDX\t5\nPDX\tBLI\t8\n")
    void Build(Graph* g, GraphDraw* gd, std::istream& in);

  }

}

#endif
