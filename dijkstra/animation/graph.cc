// graph.cc
//
// Copyright 2013 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)

// META: order of #includes: #include your own .h file first to make
// META: sure it has no unresolved dependencies, then standard library
// META: #includes in alphabetical order, then project #includes in
// META:  alphabetical order.
#include "graph.h"

#include <algorithm>
#include <climits>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

using std::endl;
using std::cerr;
using std::cout;
using std::reverse;
using std::string;
using std::vector;


namespace css343 {
  namespace dijkstra {
    // META: There's not much to this class.  A competent reviewer
    // META: might suggest it's not worth the added complexity of
    // META: setting this up.  And he/she would be right.  Done here
    // META: just to demonstrate the technique.
    class GraphImpl {
    public:
    private:
      friend class Graph;
      void dump() const;
      Vertices vertices_;
    };


    const int Vertex::INFINITE_COST = INT_MAX;


    Vertex* Graph::add_vertex(const string& label) {
      cerr << "add_vertex(" << label << ")" << endl;
      Vertex* new_vertex = new Vertex(label);
      impl()->vertices_.push_back(new_vertex);
      return new_vertex;
    }


    Vertex* Graph::find_vertex(const string& label) {
      Vertices& vertices = impl()->vertices_;
      for (Vertices::iterator it = vertices.begin(); it != vertices.end(); ++it) {
        if ((*it)->label() == label) {
          return *it;
        }
      }
      return NULL;
    }


    void Graph::get_shortest_path(Edges* path, const string& from, const string& to) {
      cerr << "Searching for shortest path from " << from << " to " << to << endl;
      Vertex* start = find_vertex(from);
      Vertex* goal = find_vertex(to);
      if ((start == NULL) || goal == NULL) {
        cerr << "missing endpoint " << from << " or " << to << endl;
        exit(EXIT_FAILURE);
      }
      get_shortest_path(path, start, goal);
    }


    // Find lowest-cost path from start to goal using Dijkstra's
    // shortest-path algorithm.
    void Graph::get_shortest_path(Edges* path, Vertex* start, Vertex* goal) {
      start->cost_ = 0;

      cerr << "Graph::get_shortest_path() start node: ";
      start->dump();
      cerr << endl;
      cerr << "Graph::get_shortest_path() goal node: ";
      goal->dump();
      cerr << endl;

      if (gd_) {
        // If generating the graphic, the start node gets labeled with
        // its cost (which should be 0) but the other nodes have
        // "infinite" cost which is uninteresting for display.
        char new_label[33];
        snprintf(new_label, 32, "%d", start->cost_);
        gd_->relabel_node(start->label_, new_label);
        gd_->highlight_node(start->label_, "green");
        gd_->highlight_node(goal->label_, "red");
        gd_->emit();
        gd_->unhighlight_node(start->label_);
        gd_->unhighlight_node(goal->label_);
      }

      // META: Note that we're copying the entire vector of vertices
      // META: with this operation.
      Vertices unvisited_vertices = impl()->vertices_;
      Vertex* current;
      while ((current = get_smallest(&unvisited_vertices)) != NULL) {
        cerr << "current node: ";
        current->dump();
        cerr << endl;
        if (gd_) {
          // Highlight the current node and emit the graph before
          // updating the costs.
          gd_->highlight_node(current->label_, "yellow");
          gd_->emit();
        }

        if (current == goal) {
          break;
        }

        // Update the cost of current's successor edges if getting to
        // the successor via current is cheaper than the previous path.
        vector<string> updated_nodes;
        for (Edges::iterator it = current->edges_.begin(); it != current->edges_.end(); ++it) {
          Edge* e = (*it);
          Vertex* v = e->to_;
          int new_cost = current->cost_ + e->weight_;
          if (new_cost < v->cost_) {
            v->cost_ = new_cost;
            v->back_edge_ = e;
            if (gd_) {
              char new_label[33];
              snprintf(new_label, 32, "%d", v->cost_);
              gd_->relabel_node(v->label_, new_label);
              gd_->highlight_node(v->label_, "magenta");
              gd_->emit();
              updated_nodes.push_back(v->label_);
            }
          }
        }

        if (gd_) {
          // Emit the graph again after we've updated the costs.
          gd_->highlight_node(current->label_, "cyan");
          gd_->emit();
          for (vector<string>::const_iterator it = updated_nodes.begin(); it != updated_nodes.end(); ++it) {
            gd_->unhighlight_node(*it);
          }
          gd_->emit();
        }
      }

      if (current != goal) {
        // META: "Not found" should not normally be a fatal error but
        // META: it's good enough for the moment.  Modify it later if
        // META: necessary.
        // META: We could also write the next line like this, which
        // META: fits into 80 columns.  Which version is more readable?
        // META: cerr
        // META:   << "cannot find path to goal node "
        // META:   << goal->label()
        // META:   << " from "
        // META:   << start->label()
        // META:   << endl;
        cerr << "cannot find path to goal node " << goal->label() << " from " << start->label() << endl;
        exit(EXIT_FAILURE);
      }

      // Recover the path from the come-from edges.  This will create
      // a vector in the reverse order so reversing it gets us the
      // forward order.
      for (Edge* back_edge = goal->back_edge_; back_edge != NULL; back_edge = back_edge->from_->back_edge_) {
        path->push_back(back_edge);
      }
      if (gd_) {
        gd_->highlight_node(start->label_, "green");
        gd_->highlight_node(goal->label_, "red");
        gd_->emit();
        for (Edges::iterator it = path->begin(); it != path->end(); ++it) {
          gd_->highlight_edge((*it)->from_->label_, (*it)->to_->label_, "red");
          gd_->emit();
        }
      }
      cerr << endl;
      // Reverse the path, so output is from start to goal
      reverse(path->begin(), path->end());
    }


    void Graph::dump() const {
      impl()->dump();
    }


    // Find the least-cost vertex in the vector.  Since the vector is
    // unordered, just do a simple linear scan.  A more sophisticated
    // approach would be to store the vertices in a priorty queue that
    // supports the decrease_key() operation.
    Vertex* Graph::get_smallest(Vertices* vertices) {
      if (vertices->empty()) {
        return NULL;
      }
      Vertices::iterator it = vertices->begin();
      Vertices::iterator smallest = it;
      int lowest_cost = (*smallest)->cost_;
      // META: commented out debug statements rather than deleting
      // META: them.  A proper logging facility would offer fine-grain
      // META: control over which messages are logged and which are
      // META: suppressed.
      // cerr << "get_smallest: initial smallest: ";
      // (*smallest)->dump();
      // cerr << endl;
      for (++it; it != vertices->end(); ++it) {
        if ((*it)->cost_ < lowest_cost) {
          smallest = it;
          //cerr << "updating smallest: ";
          //(*smallest)->dump();
          //cerr << endl;
          lowest_cost = (*smallest)->cost_;
        }
      }
      // After choosing the least-cost vertex, replace the element
      // with the last element in the vector, deleting the last
      // element.
      // META: We do this because it is much cheaper (O(1) vs. O(N))
      // META: to delete the last element of a vector than some random
      // META: element in the middle of the vector.  All this is
      // META: copacetic because we're working with pointers (shallow
      // META: copy) and the vertices are not in any particular
      // META: order.
      Vertex* v = *smallest;
      if (v->cost_ == Vertex::INFINITE_COST) {
        cerr << "no path to " << v->label_ << endl;
        return NULL;
      }
      *smallest = *(vertices->rbegin());
      vertices->pop_back();
      // cerr << "get_smallest: returning " << v << endl;
      return v;
    }


    const GraphImpl* Graph::impl() const {
      if (!p_impl_) {
        // META: This is a rare legitimate use of a const_cast.
        const_cast<Graph*>(this)->p_impl_ = new GraphImpl();
      }
      return p_impl_;
    }

    // META: two versions of essentially the same function, differing
    // META: only in constness.  They're not long enough to be worth
    // META: trying to combine them.
    GraphImpl* Graph::impl() {
      if (!p_impl_) {
        p_impl_ = new GraphImpl();
      }
      return p_impl_;
    }


    void GraphImpl::dump() const {
      for (Vertices::const_iterator vp = vertices_.begin(); vp != vertices_.end(); ++vp) {
        (*vp)->dump();
      }
    }


    void Vertex::dump() const {
      cerr << this << " \"" << label_ << "\" (cost: " << cost_ << ")" << endl;
      if (back_edge_) {
        cerr << this << "   back edge: ";
        back_edge_->dump();
      }
      for (Edges::const_iterator ep = edges_.begin(); ep != edges_.end(); ++ep) {
        (*ep)->dump();
      }
    }


    Edge* Graph::add_edge(Vertex* from, Vertex* to, int weight) {
      Edge* e = new Edge(from, to, weight);
      from->add_edge(e);
      return e;
    }


    void Edge::print() const {
      cout << from_->label() << "\t" << to_->label() << "\t" << weight_ << endl;
    }


    void Edge::dump() const {
      cerr << "   " << this
           << " " << from_->label() << "->" << to_->label()
           << " (" << weight_ << ")"
           << endl;
    }

  }
}
