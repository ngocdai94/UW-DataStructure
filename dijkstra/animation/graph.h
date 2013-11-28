// graph-draw.h
//
// Copyright 2013 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)

#if !defined(CSS343_GRAPH_H)
#define CSS343_GRAPH_H

// META: Include in the header only things that need to be in the
// META: header (i.e. that is part of the interface).
#include <cstdlib>
#include <string>
#include <vector>

#include "graph-draw.h"

namespace css343 {
  namespace dijkstra {

    // META: Forward declaration of vertex and edge classes because
    // META: edges have vertices and vertices have edges.
    class Vertex;
    class Edge;

    // META: We don't yet know what vertices and edges are, but we do
    // META: know how to create vectors of pointers to them.
    typedef std::vector<Vertex*> Vertices;
    typedef std::vector<Edge*> Edges;

    // META: the Vertex class does not deal with Edge objects, just
    // META: pointers to Edge objects.  At this point, the compiler
    // META: just knows that Edge is a type, not what it holds or how
    // META: big it might be.
    // We don't want to do much directly with vertices because most of
    // the heavy lifting will be via the Graph class.
    class Vertex {
    public:
      static const int INFINITE_COST;

      Vertex(const std::string& label) : label_(label), cost_(INFINITE_COST), back_edge_(NULL) {}
      void add_edge(Edge* e) {edges_.push_back(e);}
      const std::string& label() {return label_;}
      // META: we could also provide a print function here if it becomes
      // META: useful.  Agile programming suggests not to implement
      // META: unused functionality: do it when you need it.
      void dump() const;
    private:
      // META: tight coupling with Graph class is okay because it's
      // very local.  Graph/Vertex/Edge form a tight (cohesive) unit.
      friend class Graph;
      std::string label_;
      int cost_;
      Edge* back_edge_;
      Edges edges_;
    };

    // Similarly, edges only require a minimal interface.
    class Edge {
    public:
      Edge(Vertex* from, Vertex* to, int weight) : from_(from), to_(to), weight_(weight) {}
      // META: print() and dump() are different.  The former produces
      // META: formatted output and the latter produces debugging
      // META: output(including raw pointer addresses).
      // META: Interface design: it would be more orthogonal to
      // META: provide a function that converts to formatted string
      // META: and lets the caller figure out what to do with it.
      // META: Alternativelly, we could provide an optional ostream&
      // META: parameter, but this means bringing ostream headers into
      // META: this header.  Doing something simple now that works
      // META: well enough and making it more sophisticated later is
      // META: often (but not always) a good tradeoff.  Choices, so
      // META: many choices, even for little things.
      void print() const;
      void dump() const;
    private:
      friend class Graph;
      Vertex* from_;
      Vertex* to_;
      int weight_;
    };

    class Graph {
    public:
    Graph() : p_impl_(NULL), gd_(NULL) {}
      // Use add_vertex with care: does not check to see if there is
      // already a vertex with the same label.  You could produce a
      // graph where multiple nodes have the same label.
      // META: multiple vertices with the same name has its use cases,
      // META: so is not wrong as long as it's spelled out in the
      // META: interface contract.
      Vertex* add_vertex(const std::string& label);
      // Find a vertex with the given label.  Does not find multiple
      // occurrences of the same label.  Returns NULL if not found.
      // META: this is an expensive (linear-time) operation, but we
      // META: only need it to identify the start and end points, so
      // META: it does add to the overall asymptotic complexity.
      Vertex* find_vertex(const std::string& label);
      Edge* add_edge(Vertex* from, Vertex* to, int weight);
      // The GraphDraw field is optional, so we need a getter/setter
      void reset_graph_draw(GraphDraw* gd) {gd_ = gd;}
      GraphDraw* graph_draw() {return gd_;}
      // META: This returns void  because we'll just terminate if
      // META: there's any error.  Okay for the example program, but
      // META: you'd want more robust behavior in production code.
      // META: Two versions do not make the interface that much more
      // META: complex, but makes it more fleible for the client
      // Updates the edge vector with the shortest path between the
      // named vertices.  Client provides an empty vector.
      void get_shortest_path(Edges* path, const std::string& from, const std::string& to);
      void get_shortest_path(Edges* path, Vertex* start, Vertex* goal);
      void dump() const;
    private:
      // META: Hidden implementation: access via a pointer to another
      // META: class.  Overkill here, but done to demonstrate the
      // META: technique.  Notice that we access the p_impl_ via the
      // META: private impl() function.  This is a cheap way to
      // META: initialize it without calling new in a constructor
      // META: (which is bad because constructors are not allowed to
      // META: throw,and new throws an exception if memory allocation
      // META: fails).  An alternate approach would be to have a
      // META: factory method that creates well-formed Graph objects.
      friend class GraphImpl;
      // META: static function because it's not really a Graph
      // META: function: it's a private implementation detail.
      // Find the smallest-cost vertex in a vector.
      static Vertex* get_smallest(Vertices* vertices);
      // META: Two almost identical functions: const/non-const
      // META: versions.
      class GraphImpl* impl();
      const class GraphImpl* impl() const;
      class GraphImpl* p_impl_;
      // META: Optional GraphDraw field: a competent code reviewer
      // META: would suggest that it be put into the GraphImpl class
      // META: since it's an implementation detail.
      GraphDraw* gd_;
    };

  }  // namespace dijkstra
} // namespace css343


#endif
