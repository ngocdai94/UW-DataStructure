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

#include "priority.h"


// contract:
//
// VertexInfo holds the cost and implements methods
//   void VertexInfo::initialize_cost(Vertex* origin);
//   bool VertexInfo::update_cost(VertexInfo* predecessor, EdgeInfo* edge);
//     Returns true if cost is updated.
//   bool VertexInfo::operator<(const VertexInfo& other) const
//     compares cost of two vertices
//   string VertexInfo::repr()
//     string representation (for debugging)
//
// EdgeInfo holds segement cost used in call to
// VertexInfo::update_cost and implements methods:
//   void EdgeInfo::dump(ostream&)
//   string EdgeInfo::repr()

template <typename VertexInfo, typename EdgeInfo>
class Graph {
public:
  class Vertex;
  class Edge;
  typedef std::vector<Vertex*> Vertices;
  typedef std::vector<Edge*> Edges;

  class Edge {
  public:
    Edge(Vertex* from, Vertex* to, EdgeInfo* info)
      : from_(from), to_(to), info_(info) {
    }

    EdgeInfo* info() {return info_;}
    Vertex* from() {return from_;}
    Vertex* to() {return to_;}

    void dump(std::ostream& out) const;

  private:
    Vertex* from_;
    Vertex* to_;
    EdgeInfo* info_;

    // DISALLOW_COPY_AND_ASSIGN (do not implement)
    Edge(const Edge&);
    void operator=(const Edge&);
  };

  class Vertex : public Priority {
  public:
    Vertex(VertexInfo* info)
      : info_(info), parent_(NULL), processed_(false) {
    }

    void initialize_cost(Vertex* origin) {
      assert(origin);
      info_->initialize_cost(origin);
    }

    void update_cost(PriorityQueue<Vertex>* priority);

    template<typename F>
    void walk_minimum_path(F& action);

    bool has_valid_path() {return parent_;}

    // Compares cost of two vertices.  Only the particular application
    // knows for sure.
    bool operator<(const Vertex& other) const {
      return *info_ < *other.info_;
    }

    void add_edge(Edge* edge) {edges_.push_back(edge);}

    VertexInfo* info() {return info_;}

    std::string repr() const;

    void dump(std::ostream& out) const;

  private:
    VertexInfo* info_;
    Edges edges_;

    // Currently-known minimum-cost path from parent to current (may
    // not be overall minimum until completion of the algorithm).
    Edge* parent_;

    // Minimum cost to this vertex has been found.
    bool processed_;

    // DISALLOW_COPY_AND_ASSIGN (do not implement)
    Vertex(const Vertex&);
    void operator=(const Vertex&);
  };

  Graph() {}

  Vertex* find_minimum_path(Vertex* from, Vertex* to);

  // Call action function object for each vertex from origin to goal,
  // action is called as:
  // void action(VertexInfo* from_info, VertexInfo* to_info, EdgeInfo* edge_info));
  template <typename F>
  void walk_minimum_path(Vertex* to, F& action) {to->walk_minimum_path(action);}

  // Find some particular vertex satisfying predicate (or NULL).
  // bool predicate(VertexInfo*);
  template<typename F>
  Vertex* find_vertex(F& predicate);

  Vertex* add_vertex(VertexInfo* info);
  Edge* add_edge(EdgeInfo* info, Vertex* from, Vertex* to);

  void dump(std::ostream& out=std::cerr) const;

private:
  Vertices vertices_;

  // DISALLOW_COPY_AND_ASSIGN (do not implement)
  Graph(const Graph&);
  void operator=(const Graph&);
};


//======================================================================
// Implementation.
//======================================================================


//----------------------------------------------------------------------
// Graph functions
//----------------------------------------------------------------------

// Dijkstra's algorithm.
template<typename VertexInfo, typename EdgeInfo>
typename Graph<VertexInfo, EdgeInfo>::Vertex*
Graph<VertexInfo, EdgeInfo>::find_minimum_path(Graph<VertexInfo, EdgeInfo>::Vertex* origin,
                                               Graph<VertexInfo, EdgeInfo>::Vertex* goal) {
  assert(origin && goal);
  bool found_origin = false;
  bool found_goal = false;
  PriorityQueue<Vertex> priority;
  for (typename Vertices::iterator it = vertices_.begin(); it != vertices_.end(); ++it) {
    if ((*it) == origin) {
      found_origin = true;
    }
    if ((*it) == goal) {
      found_goal = goal;
    }
    (*it)->initialize_cost(origin);
    priority.push_back(*it);
  }
  if (!found_origin || !found_goal) {
    if (!found_origin) {
      std::cerr << "Missing origin " << origin << std::endl;
    }
    if (!found_goal) {
      std::cerr << "Missing goal " << goal << std::endl;
    }
    return NULL;
  }

  // Dijstra's greedy algorithm: keep selecting the next cheapest vertex
  // and updating its children until the goal is the the next cheapest
  // node.
  while (true) {
    assert(!priority.empty());
    Vertex* next_vertex = priority.pop();
    std::cerr << "find_minimum_path(): next_vertex: " << next_vertex->repr() << std::endl;
    if (next_vertex == goal) {
      std::cerr << "found Goal vertex: " << next_vertex->repr() << "  " << next_vertex->has_valid_path()<< std::endl;
      return next_vertex->has_valid_path() ? next_vertex : NULL;
    }
    next_vertex->update_cost(&priority);
  }
}


template <typename VertexInfo, typename EdgeInfo>
template<typename F>
typename Graph<VertexInfo, EdgeInfo>::Vertex*
Graph<VertexInfo, EdgeInfo>::find_vertex(F& predicate) {
  for (typename Vertices::iterator it = vertices_.begin(); it != vertices_.end(); ++it) {
    if (predicate((*it)->info())) {
      return *it;
    }
  }
  return NULL;
}


template <typename VertexInfo, typename EdgeInfo>
typename Graph<VertexInfo, EdgeInfo>::Vertex*
Graph<VertexInfo, EdgeInfo>::add_vertex(VertexInfo* info) {
  Vertex* new_vertex = new Vertex(info);
  vertices_.push_back(new_vertex);
  return new_vertex;
}


template <typename VertexInfo, typename EdgeInfo>
typename Graph<VertexInfo, EdgeInfo>::Edge*
Graph<VertexInfo, EdgeInfo>::add_edge(EdgeInfo* info,
                                      Vertex* from,
                                      Vertex* to) {
  Edge* new_edge = new Edge(from, to, info);
  from->add_edge(new_edge);
  return new_edge;
}


template <typename VertexInfo, typename EdgeInfo>
void
Graph<VertexInfo, EdgeInfo>::dump(std::ostream& out) const {
  for (typename Vertices::const_iterator it = vertices_.begin(); it != vertices_.end(); ++it) {
    (*it)->dump(out);
  }
}


//----------------------------------------------------------------------
// Edge functions
//----------------------------------------------------------------------

template <typename VertexInfo, typename EdgeInfo>
void Graph<VertexInfo, EdgeInfo>::Edge::dump(std::ostream& out) const {
  out << "    "
      << from_->repr()
      << " --> "
      << to_->repr()
      << "  (" << to_ << ")" << std::endl;
  info_->dump();
}


//----------------------------------------------------------------------
// Vertex functions
//----------------------------------------------------------------------

template<typename VertexInfo, typename EdgeInfo>
void Graph<VertexInfo, EdgeInfo>::Vertex::update_cost(PriorityQueue<Graph<VertexInfo, EdgeInfo>::Vertex>* priority) {
  std::cerr << "Vertex::update_cost(): updating neighbors of " << repr() << std::endl;
  processed_ = true;
  for (typename Edges::iterator it = edges_.begin(); it != edges_.end(); ++it) {
    Vertex* to = (*it)->to();
    if (!to->processed_) {
      if (to->info_->update_cost(info_, (*it)->info())) {
        std::cerr << "  Vertex::update_cost(): found better path to " << to->repr() << std::endl;
        to->parent_ = *it;
        priority->reduce(to);
      }
    }
  }
}


template<typename VertexInfo, typename EdgeInfo>
template<typename F>
void
Graph<VertexInfo, EdgeInfo>::Vertex::walk_minimum_path(F& action) {
  if (parent_) {
    Vertex* parent_vertex = parent_->from();
    parent_vertex->walk_minimum_path(action);
    action(parent_vertex->info_, info_, parent_->info());
  }
}


template<typename VertexInfo, typename EdgeInfo>
std::string
Graph<VertexInfo, EdgeInfo>::Vertex::repr() const {
  std::ostringstream buf;
  buf << info_->repr();
  return buf.str();
}


template<typename VertexInfo, typename EdgeInfo>
void
Graph<VertexInfo, EdgeInfo>::Vertex::dump(std::ostream& out) const {
  out << repr() << " (" << this << ")" << std::endl;
  for (typename Graph<VertexInfo, EdgeInfo>::Edges::const_iterator it = edges_.begin(); it != edges_.end(); ++it) {
    (*it)->dump(out);
  }
}


#endif
