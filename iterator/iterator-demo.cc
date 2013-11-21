// bst.cc: binary search tree as demo of the C++ iterator model.
//
// This program demonstrates a homespun iterator class.  A simple
// unbalanced binary search tree is used to count occurrences of each
// word in an input text.
//
// Iterators are, by far, the most important idiomatic form in C++.
// They are cleverly designed to mimic the semantics of stepping
// through an array via a pointer, for any container structure that
// supports the operation:
//   for(p = a; p != a + n; ++p) ...
//   for(it = c.begin(); it != c.end(); ++it) ...
//
// They work because it was a fundamental design goal of the language
// to make it possible to write user-defined types (i.e. classes) that
// appear syntactically just like native (built-in) types.  The key
// elements of this are references and operator overloading.
//
// The BST class contains a (stripped-down) iterator member
// class, which can be used with some standard library (STL)
// functions.
//
// Successor (next element) is found using a parent link in
// the tree node.  Refer to your favorite Data Structures and
// Alogorithms text.
//
// Copyright 2013 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)

#include <algorithm>
#include <cassert>
#include <iomanip>
#include <iostream>
#include <string>

using namespace std;


struct Data {
  Data(const string& key) : key_(key), count_(0) {}
  Data* reset(const string& key) {key_ = key; count_ = 0; return this;}
  bool operator<(const Data& rhs) {return key_ < rhs.key_;}
  void increment() {++count_;}
  // Overload the preincrement operator: same as increment() function
  // above, except for the return value which gives it behavior more
  // similar to a built-in type.
  Data& operator++() {++count_; return *this;}

  // Data field would normally be private, but this is a small program
  // so we don't require the overhead that would be essential in a
  // larger program.
  string key_;
  int count_;
};


class BST {
  struct Node;

public:
  // iterator is a member class of the enclosing BST class, similar to
  // the standard library container classes.
  class iterator {
  public:
    // Default constructor initializes its node to be a NULL pointer.
    // NULL is essentially the constant 0 used in a pointer context.
    // Why not just use 0?  YMMV.
    iterator() : node_(0) {}
    Data*& operator*(){return node_->data_;}
    bool operator==(const iterator& rhs) const {return this->node_ == rhs.node_;}
    bool operator!=(const iterator& rhs) const {return !(*this == rhs);}
    // pre-increment only.  TODO: implement the rest.
    iterator& operator++();
  private:
    friend class BST;
    // Private constructor, can only be accessed by itself or its
    // friends.  BST is the only class that needs to do this, to
    // implement the begin() method.
    iterator(Node* node) : node_(node) {}
    Node* node_;
  };

  BST() : root_(0) {}

  Data* insert(Data* data);

  iterator begin();
  iterator end();

private:
  // Node is public because this is a very small program and we don't
  // need the overhead imposed by information hiding (which is an
  // essential aspect of large-scale software systems).  Just access
  // the data directly.
  struct Node {
    Node(Data* data, Node* parent)
      : data_(data), parent_(parent), left_(0), right_(0) {
    }

    Data* insert(Data* data);

    Data* data_;
    Node* parent_;
    Node* left_;
    Node* right_;
  };

  Node* root_;
};


// This is the magic of the iterator class: data-structure aware
// implementation of the next-element operation.  Standard data
// structure stuff, but programmed idiomatically for C++.
BST::iterator& BST::iterator::operator++() {
  if (node_->right_) {
    // Node has right subtree: from the right branch, keep going left
    // as far as you can, i.e. until the node's left child is NULL.
    for (node_ = node_->right_; node_->left_ != NULL; node_ = node_->left_) {
    }
  } else {
    // No right subtree.  Successor is the first ancestor for which
    // its child is a left branch.  Find the first ancestor node that is
    // not a right child of its parent.  Either that node is the root, or its
    // parent is the successor we're looking for.  If the node is the
    // root, it's parent is NULL, which is the end() object.
    for (; node_->parent_ && (node_->parent_->right_ == node_); node_ = node_->parent_) {
    }
    node_ = node_->parent_;
  }
  return *this;
}

// Your basic BST insert function; nothing fancy.
Data* BST::Node::insert(Data* data) {
  assert(data && data_);
  if (*data < *data_) {
    if (left_) {
      return left_->insert(data);
    } else {
      left_ = new Node(data, this);
      return data;
    }
  } else if (*data_ < *data) {
      if (right_) {
        return right_->insert(data);
      } else {
        right_ = new Node(data, this);
        return data;
      }
  } else {
    // Found existing entry in tree.
    return data_;
  }
}


// Insertion into the BST: handle the special case that there is no
// root element, otherwise let the node object do the heavy lifting.
Data* BST::insert(Data* data) {
  if (!root_) {
    root_ = new Node(data, 0);
    return data;
  } else {
    return root_->insert(data);
  }
}


// Return an iterator that points to the first element in the tree,
// which is the leftmost descendant.
BST::iterator BST::begin() {
  Node* node = root_;
  for (; node->left_; node = node->left_) {
  }
  return iterator(node);
}


// C++ convention: end is one past the last element.
BST::iterator BST::end() {
  return iterator();
}


// Functor class: class that implements an operator() oveload so that
// an object of the class can be used kinda, sorta like a function.
//
// This functor prints formatted output of a Data object.  Supply an
// object of this class to std::for_each() to demonstate that our
// homegrown iterator act just like an STL container's iterator.
class Printer {
public:
  void operator()(const Data* data) {
    cout << setw(4) << data->count_ << " " << data->key_ << endl;
  }
};


int main() {
  BST bst;

  string s;
  Data* data = 0;
  while (cin >> s) {
#if 1
    // Observe that can write the data initialization more concisely
    // if we use a reset() function that does the needful.  The trick
    // is that it has to return a pointer to the object.  This is
    // exactly the sort of thing that one typically finds on the
    // second draft.  Shorter without loss of clarity is a Good Thing.
    // The "without loss of clarity" part is very, very important,
    // though.
    data = data ? data->reset(s) : new Data(s);
#else
    if (!data) {
      data = new Data(s);
    } else {
      *data = Data(s);
    }
#endif
    // Tree manipulation: basic stuff by now.
    Data* inserted = bst.insert(data);
#if 1
    // Increment the data object: we can do this because we've
    // overloaded the ++ operator.
    ++(*inserted);
#else
    // More conventional syntax for incrementing the count.
    inserted->increment();
#endif
    if (inserted == data) {
      // New element was inserted.  Since we're reusing the variable
      // on the next trip through the loop, clear it so a new object
      // will be allocated rather than clobering the one we just
      // inserted into the tree.
      data = 0;
    }
  }
  if (data) {
    // One data object was left over.  RAII would have taken care of
    // this automagically if a smart pointer was used, but this is
    // such a small program, we don't need it for this tiny little
    // thing.
    delete data;
  }

#if 1
  // Use the standard library function with a home-grown iterator!
  for_each(bst.begin(), bst.end(), Printer());
#else
  // Idiomatic construct for stepping through the elements of a data
  // struture, but the data structure is home-grown! Nothing magic
  // about the STL!
  for (BST::iterator it = bst.begin() ; it != bst.end(); ++it) {
    cout << setw(4) << (*it)->count_ << " " << (*it)->key_ << endl;
  }
#endif

  // Being lazy here.  Should return EXIT_SUCCESS which is #define'd
  // to... 0.
  return 0;
}
