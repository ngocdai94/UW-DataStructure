// build.cc
//
// See build.h for description
//
// Copyright 2013 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)

#include "build.h"

#include <cassert>
#include <cstdio>
#include <map>
#include <string>

#include "graph.h"
#include "graph-draw.h"


// META: Style alternative: instead of using the entire std::
// META: namespace, we just use the specific names that we actually
// META: want to use.
using std::istream;
using std::map;
using std::string;


namespace css343 {
  namespace dijkstra {
    // META: VertexByName is more descriptive and slighly more concise
    // META:than map<string, Vertex*>.
    typedef map<string, Vertex*> VertexByName;

    // Check to see whether we've already created a vertex with the
    // given name.  If so, return that vertex; otherwise create a new
    // graph vertex and track it's name.
    // META: Notice that the vertex-by-name dictionary is not a
    // META: property of the graph.  We track it here in the builder,
    // META: then discard it once the graph is built.
    // META: Also notice that the function takes the vertices as a
    // META: parameter so it doen't have to keep state information.
    // META: An alternate approach would be to create a class that
    // META: encapsulates the vertex-by-name dictionary.  The choice
    // META: does not matter here because it's small and local.
    static Vertex* get_vertex_by_name(VertexByName* vertices, Graph* g, GraphDraw* gd, const string& name) {
      // META: all paths assign something to v, so we don't really
      // META: care that it's not initialized yet.
      Vertex*& v = (*vertices)[name];
      if (!v) {
        // The vertex is not already present, so create a new one.
        // Since v is a reference, assigning to v automagically
        // updates the vertices map.
	v = g->add_vertex(name);
	if (gd) {
	  gd->add_node(name);
        }
      }
      return v;
    }

    // META: This is not particularly robust.  Bad input should never
    // META: cause an uncontrolled crash in a program.  Useful
    // META: diagnostic messages helps you track down errors.
    void Build(Graph* g, GraphDraw* gd, istream& in) {
      assert(g);

      VertexByName vertices;
      while (in.good() && !in.eof()) {
	string from;
	string to;
	int weight;

	in >> from;
	// META: Remember De Morgan's laws?
	// META (!in.good() || in.eof()) is the same as
	// META: !(in.good() && ! in.eof()).
	if (!in.good() || in.eof()) {
	  break;
	}
	in >> to >> weight;

	Vertex* v1 = get_vertex_by_name(&vertices, g, gd, from);
	Vertex* v2 = get_vertex_by_name(&vertices, g, gd, to);
	g->add_edge(v1, v2, weight);
	if (gd) {
	  // META: Notice that we have two completely different
	  // META: interfaces for building a graph because the two
	  // META: different pieces were provided by different
	  // META: vendors, were written by different people, was
	  // META: written by one person for different purposes, or
	  // META: was written by one person with Multiple Personality
	  // META: Disorder.  Deal with it.
	  // META: Also notice that weight_buf should be large enough
	  // META: to hold the string representation of a 32-bit
	  // META: number, but it's not a very robust way of doing it.
	  // META: A mature (reasonably well-established) organization
	  // META: would have a bullet-proof (i.e. well-tested)
	  // META: library this kind of routine in their codebase.
	  char weight_buf[33];
	  snprintf(weight_buf, 32, "%d", weight);
	  gd->add_edge(from, to, string(weight_buf));
	}
      }
    }


  }  // namespace dijkstra
}  // namespace css343
