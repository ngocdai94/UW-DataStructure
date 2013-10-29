// wordcount-btree.cc
//
// 2-3 tree (B Tree) implementation for counting occurences of
// words in a text.
//
// This is written as a single monolithic file simply to demonstrate
// the concept and is not intended to be production-grade code.
// Normally, the B Tree implementation would be in separate files from
// the word-count application.
//
// Comments marked "META:" would not normally be present in the source
// file but are added to clarify the code for someone learning the
// data structure or C++.
//
// Copyright 2013 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)


// META: standard library include files in alphabetical order.
#include <cassert>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>

using namespace std;


// B Tree dictionary template.  Uses reference semantics.  By B Tree,
// we really mean 2-3 tree, a special case of the more general B Tree
// structure (all 2-3 trees are B Trees; not all B Trees are 2-3
// trees).
//
// There must be a defined comparison function
//    bool operator<(const Thing& d1, const Thing& d2)
//
// Note that all comparisons are based on operator<.
//
// META: Thing& is a reference to type Thing, which is a pointer to
// Thing, with automatic dererencing, e.g:
//   int x = 42;
//   int* xptr = &x;
//   *xptr = 99
//   // x is now 99
//   int& xref = x;
//   xref = 27;
//   // x is now 27
//
//
// META: reference semantics: stores pointers and assumes the caller
// manages the underlying  data).  This is a different design
// philosopy than the value semantics of the standard template
// library.  There are tradeoffs between the two approaches, but this
// approach is better for understanding pointers.
//
// META: "dictionary template" -- would normally be in in a separate
// file,  say btree.h
//
// META: BTree provides the public interface but delegates most of
// the work to the private Node type.
//
// META: we only implement the contructor, insert and walk functions;
// consider adding lookup and delete functions.
template<typename Thing>
class BTree {
public:
  BTree();

  // returns the thing found in the tree if already present; otherwise
  // returns data.
  Thing* insert(Thing* data);

  // Visit every Thing in the tree, applying f to each thing.  Functor
  // f must take a single Thing* argument and return void.
  template<typename F> void walk(F& f);

  // Display tree structure for debugging.
  void dump();

  // Validation: verify that the tree is well-formed:
  //   o all nodes have data1__
  //   o non-leaf nodes with only one key have left_ and  middle
  //     children but no right child
  //   o non-leaf nodes with two keys have all three children
  //   o all leaves are at the same depth.
  // META: left as an exercise.  Implement validate() function: check
  // that every/ leaf is at the same depth.
  // META: this function is unimplemented.  It's not an error as long
  // as it never gets called.
  bool validate();

private:
  class Node {
  public:
    // META: constructor uses default arguments, which must be
    // specified here (not at the point where the constructor is
    // defined).  The use case is that we'll either insert data into a
    // new leaf node (data2_ and all children NULL), or construct a
    // node from splitting another node, in which case data2_ and
    // right_ will be NULL.
    Node(Thing* data, Node* left=NULL, Node* middle=NULL);

    // Insert/find data.  If data is already present in the tree found
    // will be set to point to the existing data; otherwise, found
    // will point to the existing data.
    // META: This is one way of doing it.  As you can see from the use
    // case in the main program below, it's a bit clunky compared to
    // the interface provided by the std::map template.
    Node* insert(Thing* data, Thing** found);

    // f is a function object (functor) with signature void f(Thing* data)
    // applied to every datum in the tree (in sorted order).
    template<typename F> void walk(F& f);

    // META: dump functions make debugging easier.  Generally, debug
    // output should go to standard error, not standard output.  A
    // more sophisticated production-grade logging facility is
    // overkill for this demonstration.
    void dump(int depth=0, bool recursive=true, ostream& out=cerr);

  private:
    bool is_leaf() {return left_ == NULL;}

    // Helper functions for insert()
    Node* insert_leaf(Thing* data, Thing**found);
    Node* insert_left(Thing* data, Thing**found);
    Node* insert_middle(Thing* data, Thing**found);
    Node* insert_right(Thing* data, Thing**found);

    // META: since we are restricting ourselves to the 2-3 tree
    // special case, we can use left/middle/right as explicit fields.
    // We could also declare the fields as
    //   Thing* data[2]
    //   Node* children[3];
    // and define enum values LEFT, MIDDLE, RIGHT
    // but this does not add clarity to the code.  Instead, we treat
    // left, middle, and right as distinct cases.  This approach does
    // not scale to the general B Tree implementation.
    Thing* data1_;
    Thing* data2_;
    Node* left_;
    Node* middle_;
    Node* right_;
  };

  // META: the only data field of the BTree class is a single node
  // pointer.
  // META: naming convention.  Member variables have trailing
  // underscore to distinguish from local variables.
  Node* root_;
};


// META: following template methods implementations would normally be
// in btree.h  or btree-impl.h.  They are not part of the interface
// but need to be #include'ed into any file that uses them because the
// compiler needs to be able to generate code in the translation
// unit.


// META: trivial constructor, could be placed inline in the class
// body.  Doing so saves couple of lines but mixes interface and
// implementation.  Since the implementation is trivial; any competent
// programmer could ignore detail.
template<typename Thing>
BTree<Thing>::BTree(): root_(NULL) {}


// Trivial insert function.  The heavy lifting is delegated to
// BTree::Node::insert().
//
// META: BTree::Node::insert() returns a new node if the current node
// must be split.  The new node is the root of the bush, which here
// becomes the new root of the entire tree.
//
// META: note the second parameter to the insert function, found is
// a pointer to matching data found in the tree if it is already
// present, or the incoming data if it gets inserted.  In order to
// change the value of found, we have to pass a pointer (hence,
// pointer-to-pointer).
//
// META: note the special case for creating a new root when the tree
// has no data at all.
template<typename Thing>
Thing* BTree<Thing>::insert(Thing* data) {
  if (root_) {
    Thing* found;
    Node* new_root = root_->insert(data, &found);
    if (new_root) {
      root_ = new_root;
    }
    return found;
  } else {
    root_ = new Node(data);
    return data;
  }
}


// META: walk function visites every data in order and calls function
// f on the data.  f is a function object (an object that implements
// operator() as a method), AKA functor.  Visiting every node is known
// as walking the tree (or walking through the tree).
//
// META: again, note that the heavy lifting is done by
// BTree::Node::walk().
template<typename Thing>
template<typename F>
void BTree<Thing>::walk(F& f) {
  if (root_) {
    root_->walk(f);
  }
}


template<typename Thing>
void BTree<Thing>::dump() {
  root_->dump();
}


// META: trivial constructor for BTree::Node class, could be in the
// class body.
template<typename Thing>
BTree<Thing>::Node::Node(Thing* data, Node* left, Node* middle)
  : data1_(data), data2_(NULL), left_(left), middle_(middle), right_(NULL) {
}


// Inserting into the leaf node: we have to either fit it into the
// right place or split the leaf node.  If splitting the leaf node, we
// have to figure out which is the middle value.
//
// META: each case is slightly different.  We could try to combine the
// cases, but it's doesn't save any effort.  We would have to combine
// the cases in the generalized BTree framework since this approach
// does not scale.
template<typename Thing>
typename BTree<Thing>::Node* BTree<Thing>::Node::insert_leaf(Thing* data, Thing** found) {
  if (data2_) {
    // Node is full.  If data is not already in this node, we'll need
    // to split the node.
    if (*data < *data1_) {
      // data < data1_ < data2: middle node is data1_
      Node* new_sibling = new Node(data);
      Node* new_root = new Node(data1_, new_sibling, this);
      data1_ = data2_;
      data2_ = NULL;
      *found = data;
      return new_root;
    } else if ((*data1_ < *data) && (*data < *data2_)) {
      // data1_ < data < data2_: middle node is data
      Node* new_sibling = new Node(data2_);
      data2_ = NULL;
      Node* new_root = new Node(data, this, new_sibling);
      *found = data;
      return new_root;
    } else if (*data2_ < *data) {
      // data1_ < data2_ < data: middle node is data2_
      Node* new_sibling = new Node(data);
      Node* new_root = new Node(data2_, this, new_sibling);
      data2_ = NULL;
      *found = data;
      return new_root;
    } else if (*data1_ < *data) {
      // data == data2_
      // META: we've determined equality using only operator<().  The
      // cases have to be examined in this particular order to make it
      // work.
      *found = data2_;
      return NULL;
    } else {
      // data == data1_
      *found = data1_;
      return NULL;
    }
  } else {
    // There is room in the node for data.
    // If data != data1_, we either have to make data2_ = data or
    // shift data1_ into data2_ and move data into the position of
    // data1_.
    if (*data < *data1_) {
      data2_ = data1_;
      data1_ = data;
      *found = data;
    } else if (*data1_ < *data) {
      data2_ = data;
      *found = data;
    } else {
      *found = data1_;
    }
    return NULL;
  }
}


// Already determined that data < data1_ and this is a non-leaf node.
// Insert data into the left subtree.  If the recursive call returns
// an overflow node, insert it into this if it fits, or split this and
// pass the overflow back to the caller.
template<typename Thing>
typename BTree<Thing>::Node* BTree<Thing>::Node::insert_left(Thing* data, Thing** found) {
  Node* overflow = left_->insert(data, found);
  // META: any overflow coming from the left subtree must be less than
  // data1_.
  if (overflow) {
    if (data2_) {
      // Overflow node will be left child; make this node the middle
      // child.
      Node* new_root = new Node(data1_, overflow, this);
      data1_ = data2_;
      data2_ = NULL;
      left_ = middle_;
      middle_ = right_;
      right_ = NULL;
      return new_root;
    } else {
      data2_ = data1_;
      data1_ = overflow->data1_;
      right_ = middle_;
      middle_ = overflow->middle_;
      left_ = overflow->left_;
      delete overflow;
      return NULL;
    }
  } else {
    return NULL;
  }
}


// data1_ < data and if data2_ is non-null, data < data2_.
template<typename Thing>
typename BTree<Thing>::Node* BTree<Thing>::Node::insert_middle(Thing* data, Thing** found) {
  Node* overflow = middle_->insert(data, found);
  if (overflow) {
    if (data2_) {
      Node* new_sibling = new Node(data2_, overflow->middle_, right_);
      data2_ = NULL;
      right_ = NULL;
      middle_ = overflow->left_;
      overflow->left_ = this;
      overflow->middle_ = new_sibling;
      return overflow;
    } else {
      data2_ = overflow->data1_;
      middle_ = overflow->left_;
      right_ = overflow->middle_;
      delete overflow;
      return NULL;
    }
  } else {
    return NULL;
  }
}


// data2_ < data (there must be a data2_ and if there is an overflow,
// this must automatically split and the middle key is data2_.
template<typename Thing>
typename BTree<Thing>::Node* BTree<Thing>::Node::insert_right(Thing* data, Thing** found) {
  Node* overflow = right_->insert(data, found);
  if (overflow) {
    Node* new_root = new Node(data2_, this, overflow);
    data2_ = NULL;
    right_ = NULL;
    return new_root;
  } else {
    return NULL;
  }
}


// META: Node insert function just determines which case to apply and
// then farms that work off to the right helper function.  This adds a
// few extra lines for the function declarations, but if we simply
// made a single monolithic function, it would be about 120 lines and
// considerably more painful to read than 5 shorter functions.
template<typename Thing>
typename BTree<Thing>::Node* BTree<Thing>::Node::insert(Thing* data, Thing** found) {
  if (is_leaf()) {
    return insert_leaf(data, found);
  } else if (*data < *data1_) {
    return insert_left(data, found);
  } else if (*data1_ < *data && (!data2_ || *data < *data2_)) {
    return insert_middle(data, found);
  } else if (data2_ && *data2_ < *data) {
    return insert_right(data, found);
  } else if (*data1_ < *data) {
    *found = data2_;
    return NULL;
  } else {
    *found = data1_;
    return NULL;
  }
}


// In-order traversal, (2-3 tree)-style.  User supplied action f
// applied to each entry in sorted order.
template<typename Thing>
template<typename F>
void BTree<Thing>::Node::walk(F& f) {
  if (left_) {
    left_->walk(f);
  }
  assert(data1_);
  f(data1_);
  if (middle_) {
    middle_->walk(f);
  }
  if (data2_) {
    f(data2_);
  }
  if (right_) {
    right_->walk(f);
  }
}


// Dump function for debugging: special case of walk function. The
// depth parameter makes output more readable by indenting according
// to tree height.  Assumes class Thing has a print() method.
template<typename Thing>
void BTree<Thing>::Node::dump(int depth, bool recursive, std::ostream& out) {
  for (int i=0; i < depth; i++) {
    out << "+---";
  }
  out << " " << this << " (" << left_ << ", " << middle_ << ", " << right_ << ") ";
  if (data1_) {
    // META: if the tree is well-formed, there absolutely _must_ be a
    // data1_.  Since this routine is used for debugging, it needs to
    // be more robust.
    data1_->print(out);
  }
  if (data2_) {
    data2_->print(out);
  }
  out << endl;
  if (recursive && left_) {
    left_->dump(depth + 1);
  }
  if (recursive && middle_) {
    middle_->dump(depth + 1);
  }
  if (recursive && right_) {
    right_->dump(depth + 1);
  }
}


// META: from here onward is the application.


// META: the template above knows nothing of word counting; all it
// needs is a comparison function.
//
// META: in order to work with the BTree class, you need to create a
// class containing the key and value.  Contrast this with
// wordcount-map.cc.  Considerably more plumbing is required here
// because std::map has a more sophisticated interface using std::pair
// and iterators.

// META: Mydata is trivial.
class MyData {
public:
  MyData(string& word): word_(word), count_(0) {}
  void increment() {++count_;}
  void print(ostream& out) {out << setw(4) << count_ << "  " << word_;}
private:
  friend bool operator<(const MyData& d1, const MyData& d2);
  string word_;
  int count_;
};


bool operator<(const MyData& d1, const MyData& d2) {
  return d1.word_ < d2.word_;
}


// META: contrast the plumbing needed to use the walk function with
// STL iterators.
class MyFunction {
public:
  explicit MyFunction(): ordinal_(0) {}
  void operator()(MyData* data) {
    if (data) {
      cout << setw(4) << ++ordinal_ << " ";
      data->print(cout);
      cout << endl;
    }
  }
private:
  int ordinal_;
};


int main () {
  BTree<MyData> t;
  string s;
  while (getline(cin, s)) {
    MyData* tmpdata = new MyData(s);
    cerr << endl << endl << "********* inserting: " << s << endl;
    MyData* found = t.insert(tmpdata);
    // META: increment the count for the object in the tree regardless
    // of whether it was already present (found != tmpdata) or newly
    // added (found == tmpdata) is immaterial.
    // META: dumping the tree after every insert turns the nifty
    // O(N log N) algorithm into a messy O(N**2).  Try uncommenting
    // the next line out and comparing run-time performance for a large
    // file, say 20,000 words.
    // t.dump();
    found->increment();
    if (found != tmpdata) {
      // We don't need tmpdata because the word was already in the
      // tree.  Otherwise, the tree owns the pointer.
      //
      // TODO: extra allocation/deletion can be avoided by
      // restructuring the code slightly and making it possible to
      // reset a MyData object.
      //
      // META: the above TODO will never happen because it's not a
      // performance bottleneck, so not worthwhile making working code
      // more complex.
      delete tmpdata;
    }
  }
  cerr << endl << endl << "********* final tree: " << s << endl;
  t.dump();
  MyFunction f;
  t.walk(f);
  return 0;
}
