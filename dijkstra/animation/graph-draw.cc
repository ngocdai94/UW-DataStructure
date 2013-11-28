// graph-draw.cc
//
// Copyright 2013 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)

#include "graph-draw.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

using namespace std;


namespace css343 {
  const char* GraphDraw::UNHIGHLIGHT = "NONE";


  int GraphDraw::emit() {
    stringstream filename;
    filename << basename_;
    filename << setw(4) << setfill('0') << ++frame_;
    filename << ".dot";
    ofstream out(filename.str().c_str());
    return emit(out);
  }

  // Sample output:
  // digraph {
  //    bfi[label="bfi"];
  //    bli[label="bli", style=filled, fillcolor="yellow"];
  //    geg[label="geg"];
  //    pdx[label="pdx\nPortland"];
  //    sea[label="sea"];
  //    ykm[label="ykm", style=filled, fillcolor="red"];
  //    bfi -> geg [label="8"];
  //    bfi -> pdx [label="6"];
  //    bli -> bfi [label="6"];
  //    bli -> geg [label="16"];
  //    bli -> pdx [label="4"];
  //    bli -> sea [label="16"];
  //    geg -> ykm [label="4"];
  //    pdx -> geg [label="2"];
  //    sea -> geg [label="4", color="green", penwidth=4, arrowsize=2];
  //    sea -> pdx [label="6"];
  //    sea -> ykm [label="8"];
  //    ykm -> bfi [label="10"];
  //    ykm -> bli [label="8"];
  //    ykm -> sea [label="4"];
  // }
  int GraphDraw::emit(ostream& out) {
    out << "digraph {" << endl;

    for (Vertices::const_iterator vertex_it = vertices_.begin(); vertex_it != vertices_.end(); ++vertex_it) {
      const string& id = vertex_it->first;
      const string& label = vertex_it->second->label();
      const string& color = vertex_it->second->color();
      out << "   " << id << "[label=\"" << id;
      if (label != "") {
	out << "\\n" << label;
      }
      out << "\"";
      if (color != UNHIGHLIGHT) {
	out << ", style=filled, fillcolor=\"" << vertex_it->second->color() << "\"";
      }
      out << "];" << endl;
    }

    for (Edges::const_iterator edge_it = edges_.begin(); edge_it != edges_.end(); ++edge_it) {
      Edge* edge = edge_it->second;
      out << "   " << edge->from() << " -> " << edge->to() << " [label=\"" << edge->label() << "\"";
      if (edge->color() != UNHIGHLIGHT) {
	out << ", color=\"" << edge->color() << "\"";
        out << ", penwidth=4, arrowsize=2";
      }
      out << "];" << endl;
    }

    out << "}" << endl;

    return 0;
  }


  void GraphDraw::add_node(const string& id) {
    Vertex*& v= vertices_[id];
    if (v) {
      cerr << "GraphDraw::add_node: adding " << id << " already present" << endl;
      return;
    }
    // Since v is a reference, assigning to v automagically updates
    // the vertices_ map.
    v = new Vertex(id);
  }


  void GraphDraw::add_edge(const string& v1, const string& v2, const string& label) {
    Vertex* v1_vertex = vertices_[v1];
    if (!v1_vertex) {
      cerr << "adding edge (" << v1 << ", " << v2 << "): " << v1 << " not found" << endl;
      return;
    }
    Vertex* v2_vertex = vertices_[v2];
    if (!v2_vertex) {
      cerr << "adding edge (" << v1 << ", " << v2 << "): " << v2 << " not found" << endl;
      return;
    }
    Edge*& e = edges_[VertexPair(*v1_vertex, *v2_vertex)];
    if (e) {
      cerr << "adding edge (" << v2 << ", " << v2 << "): edge already present" << endl;
      return;
    }
    // Reference assignment automagically updates the map.
    e = new Edge(v1, v2, label);
  }


  void GraphDraw::unhighlight_nodes() {
    for (Vertices::iterator it = vertices_.begin(); it != vertices_.end(); ++it) {
      it->second->set_color(UNHIGHLIGHT);
    }
  }


  void GraphDraw::unhighlight_edges() {
    for (Edges::iterator it = edges_.begin(); it != edges_.end(); ++it) {
      it->second->set_color(UNHIGHLIGHT);
    }
  }


  void GraphDraw::relabel_node(const string& id, const string& label) {
    Vertex*& v = vertices_[id];
    if (!v) {
      cerr << "attempting to relabel unknown node " << id << endl;
      return;
    }
    v->set_label(label);
  }


  void GraphDraw::highlight_node(const string& id, const string& color) {
    Vertex*& v = vertices_[id];
    if (!v) {
      cerr << "highlighting nonexistent node: " << id << endl;
      return;
    }
    v->set_color(color);
  }


  void GraphDraw::unhighlight_node(const string& id) {
    highlight_node(id, UNHIGHLIGHT);
  }


  void GraphDraw::highlight_edge(const string& label1, const string& label2, const string& color) {
    Edge*& e = edges_[VertexPair(Vertex(label1), Vertex(label2))];
    if (!e) {
      cerr << "highlighting nonexistent edge: (" << label1 << ", " << label2 << ")" << endl;
      return;
    }
    e->set_color(color);
  }


  void GraphDraw::unhighlight_edge(const string& label1, const string& label2) {
    highlight_edge(label1, label2, UNHIGHLIGHT);
  }


  bool operator<(const GraphDraw::VertexPair& edge1, const GraphDraw::VertexPair& edge2) {
    if (edge1.first.id_ < edge2.first.id_) {
      return true;
    } else if ((edge1.first.id_ == edge2.first.id_) &&
	       (edge1.second.id_ < edge2.second.id_)) {
      return true;
    } else {
      return false;
    }
  }


}  // namespace css343
