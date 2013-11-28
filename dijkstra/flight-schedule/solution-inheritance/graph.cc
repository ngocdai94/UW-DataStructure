// graph.cc
//
// Generic graph for demonstrating Dijkstra's algorithm
//
// Copyright 2013 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)

#include "graph.h"

#include <algorithm>
#include <cassert>
#include <sstream>

using namespace std;


// Simple linear scan to extract minimum element.
// TODO: replace with priority queue for better performance.
static Vertex* next_unprocessed(Vertices* unprocessed) {
  assert(!unprocessed->empty());
  Vertices::iterator min_vertex = unprocessed->begin();
  for (Vertices::iterator it = min_vertex + 1; it != unprocessed->end(); ++it) {
    if (**it < **min_vertex) {
      min_vertex = it;
    }
  }
  // Since the vector is unordered, we can move the last vertex into
  // the position of the found minimum and then delete the last
  // vertex.  This is O(1) instead of an O(N) delete-from-somewhere-
  // in-the-middle.  Asymptotically, this is no gain because we're
  // already doing an O(N) scan, but why do double the work
  // unnecessarily?
  Vertex* result = *min_vertex;
  *min_vertex = *(unprocessed->rbegin());
  unprocessed->pop_back();
  return result;
}


//----------------------------------------------------------------------
// Vertex functions
//----------------------------------------------------------------------

Vertex::~Vertex() {
  for (Edges::iterator it = edges_.begin(); it != edges_.end(); ++it) {
    delete *it;
  }
}


Edge* Vertex::add_edge(Edge* e) {
  assert(e && (e->from() == this));
  edges_.push_back(e);
  return e;
}


void Vertex::relax() {
  for (Edges::iterator it = edges_.begin(); it != edges_.end(); ++it) {
    Vertex* to = (*it)->to();
    if (to->update_cost(*it)) {
      to->parent_ = (*it);
    }
  }
}


void Vertex::walk_minimum_path(EdgeAction* do_before, EdgeAction* do_after) {
  if (parent_) {
    if (do_before) {
      do_before(parent_);
    }
    parent_->from()->walk_minimum_path(do_before, do_after);
    if (do_after) {
      do_after(parent_);
    }
  }
}


string Vertex::repr() const {
  stringstream s;
  s << "vertex(" << this << ")";
  return s.str();
}


void Vertex::dump(ostream& out) const {
  out << repr() << endl;
  for (Edges::const_iterator it = edges_.begin(); it != edges_.end(); ++it) {
    (*it)->dump(out);
  }
}


//----------------------------------------------------------------------
// Edge functions
//----------------------------------------------------------------------

Edge::Edge(Vertex* from, Vertex* to) : from_(from), to_(to) {
  assert(from_ && to_);
  from_->add_edge(this);
}


string Edge::repr() const {
  stringstream s;
  s << "Edge(" << this << ") "
    << from_->repr() << " --> " << to_->repr();
  return s.str();
}


void Edge::dump(ostream& out) const {
  out << "    " << repr() << endl;
}


//----------------------------------------------------------------------
// Graph functions
//----------------------------------------------------------------------

Graph::~Graph() {
  for (Vertices::iterator it = vertices_.begin(); it != vertices_.end(); ++it) {
    delete *it;
  }
}

Vertex* Graph::add_vertex(Vertex* v) {
  vertices_.push_back(v);
  return v;
}


Vertex* Graph::find_vertex(bool (*predicate)(const Vertex*)) {
  for (Vertices::iterator it = vertices_.begin(); it != vertices_.end(); ++it) {
    if (predicate(*it)) {
      return *it;
    }
  }
  return NULL;
}


// Dijkstra's algorithm.
Vertex* Graph::find_minimum_path(Vertex* origin, Vertex* goal) {
  assert(origin && goal);
  bool found_origin = false;
  bool found_goal = false;

  vector<Vertex*> unprocessed;
  for (typename Vertices::iterator it = vertices_.begin(); it != vertices_.end(); ++it) {
    if ((*it) == origin) {
      found_origin = true;
    }
    if ((*it) == goal) {
      found_goal = true;
    }
    (*it)->initialize_cost(origin);
    unprocessed.push_back(*it);
  }
  if (!found_origin || !found_goal) {
    if (!found_origin) {
      cerr << "Missing origin " << origin << endl;
    }
    if (!found_goal) {
      cerr << "Missing goal " << goal << endl;
    }
    return NULL;
  }

  while(!unprocessed.empty()) {
    Vertex* v = next_unprocessed(&unprocessed);
    assert(v);
    cerr << "find_minimum_path(): next_vertex: " << v->repr() << endl;
    if (v == goal) {
      cerr << "found Goal vertex: " << v->repr() << "  " << v->has_valid_path() << endl;
      return v->has_valid_path() ? v : NULL;
    }
    v->relax();
  }
  cerr << "unable to find minimum path" << endl;
  return NULL;
}


void Graph::dump(ostream& out) const {
  out << "graph(" << this << ")" << endl;
  for (Vertices::const_iterator it = vertices_.begin(); it != vertices_.end(); ++it) {
    (*it)->dump(out);
  }
}
