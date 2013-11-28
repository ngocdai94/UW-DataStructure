// graph.h
//
// Generic graph for demonstrating Dijkstra's algorithm
//
// Copyright 2013 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)

#if !defined(CSS343_GRAPH_H)
#define CSS343_GRAPH_H

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <istream>
#include <map>
#include <sstream>
#include <string>
#include <vector>


class Vertex;
class Edge;

typedef std::vector<Vertex*> Vertices;
typedef std::vector<Edge*> Edges;

typedef void EdgeAction(Edge*);


// Abstract base class: a vertex is a set of edges.  Client code
// subclasses Vertex and implements methods (and any additional data
// members) for managing the "cost" of the node:
//    initialize_cost()
//    update_cost()
//    operator<()
class Vertex {
public:
  Vertex() {}
  virtual ~Vertex();

  // Graph construction: an Edge is a pair of vertices from and to.
  // This vertex must be the from vertex of the edge.
  virtual Edge* add_edge(Edge* edge);

  // Client (subclass) cost management.
  //
  // initialize_cost() takes the origin vertex as a parameter because
  // the cost of the origin is zero and the cost of non-origin
  // vertices is infinity (for client-defined values of zero and
  // infinity).
  //
  // update_cost() returns true if the vertex cost was updated.  The
  // argument is the incoming edge.  The predecessor vertex can be
  // found from the edge.
  //
  // operator<() compares vertices by cost.
  virtual void initialize_cost(Vertex* origin) = 0;
  virtual bool update_cost(Edge* edge) = 0;
  virtual bool operator<(const Vertex& other) = 0;

  // Minimum-path functions.
  void relax();
  bool has_valid_path() const {return parent_;}
  virtual void walk_minimum_path(EdgeAction* do_before, EdgeAction* do_after);

  // Formatted output & debugging.
  virtual std::string str() const {return repr();}
  virtual std::string repr() const;
  virtual void dump(std::ostream& out=std::cerr) const;

private:
  Edges edges_;

  // Currently-known minimum-cost path from parent to current (may
  // not be overall minimum until completion of the algorithm).
  Edge* parent_;

  // DISALLOW_COPY_AND_ASSIGN (do not implement)
  Vertex(const Vertex&);
  void operator=(const Vertex&);
};


// Abstract base class: an edge is a pair of vertices, predecessor &
// successor (or from and to).  Edges have weight, but it's up to the
// client subclass to define weight in application-specific terms.
class Edge {
public:
  Edge(Vertex* from, Vertex* to);
  virtual ~Edge() {}

  Vertex* from() {return from_;}
  Vertex* from() const {return from_;}
  Vertex* to() {return to_;}
  Vertex* to() const {return to_;}

  virtual std::string str() const {return repr();}
  virtual std::string repr() const;
  virtual void dump(std::ostream& out=std::cerr) const;

private:
  Vertex* from_;
  Vertex* to_;

  // DISALLOW_COPY_AND_ASSIGN (do not implement)
  Edge(const Edge&);
  void operator=(const Edge&);
};


// Concrete graph class: a graph is a set of vertices (each of which
// has edges).  May be subclassed if necessary but subclassing should
// not be necessary.
class Graph {
public:
  Graph() {}
  virtual ~Graph();

  // Graph construction.
  Vertex* add_vertex(Vertex* v);

  // Find a vertex satisfying given predicate (or return NULL if not
  // found).
  Vertex* find_vertex(bool (*predicate)(const Vertex*));

  Vertex* find_minimum_path(Vertex* from, Vertex* to);

  virtual void dump(std::ostream& out=std::cerr) const;

private:
  Vertices vertices_;

  // DISALLOW_COPY_AND_ASSIGN (do not implement)
  Graph(const Graph&);
  void operator=(const Graph&);
};


#endif
