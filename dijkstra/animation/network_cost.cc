// network_cost.cc
//
// Per CSS 343 Assignment 4 requirements: read a list of weighted
// edges and computes the least-path cost between start and goal
// vertices.
//
// Copyright 2013 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>

#include "build.h"
#include "graph.h"

using namespace std;
using namespace css343;
using namespace css343::dijkstra;


void usage(const char* name) {
  cerr << "Usage: " << name << " [-o frame] from to < graphdata.txt" << endl;
  exit(EXIT_FAILURE);
}


int main(int argc, const char *argv[]) {
  const char* from = NULL;
  const char* to = NULL;
  const char* dot_file = NULL;

  // META: ad hoc argument processing at its ugliest.  A mature shop
  // META: would have a flags library for this sort of thing.  A less
  // META: mature shop would consider using the getopt library.
  if (argc < 3) {
    usage(argv[0]);
  }
  if (strcmp(argv[1], "-o") == 0) {
    if (argc != 5) {
      usage(argv[0]);
    }
    dot_file = argv[2];
    from = argv[3];
    to = argv[4];
  } else {
    if (argc != 3) {
      usage(argv[0]);
    }
    from = argv[1];
    to = argv[2];
  }

  Graph g;
  GraphDraw* gd = NULL;
  if (dot_file) {
    gd = new GraphDraw(dot_file);
    g.reset_graph_draw(gd);
  }
  Build(&g, gd, cin);
  cerr << endl;
  g.dump();
  cerr << endl;

  Edges path;
  g.get_shortest_path(&path, from, to);
  for (Edges::iterator it = path.begin(); it != path.end(); ++it) {
    (*it)->print();
  }

  return EXIT_SUCCESS;
}
