// Copyright 2012 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)
//

#include "huffman_tree.h"

#include <cassert>
#include <cctype>
#include <climits>
#include <cstdlib>
#include <iomanip>

#include <queue>
#include <vector>

using namespace std;


class HuffmanNode {
public:
  HuffmanNode(unsigned char symbol, int frequency)
    : symbol_(symbol), frequency_(frequency), left_(NULL), right_(NULL) {
  }

  HuffmanNode(HuffmanNode* left, HuffmanNode* right)
    : symbol_(0), frequency_(left->frequency_ + right->frequency_), left_(left), right_(right) {
    assert(left);
    assert(right);
  }

  int frequency() const {return frequency_;};

  void encode(CodedSymbol coded_symbol, CodingTable* encoding);

  void dump(int level=0, ostream& out=cerr) const;

  HuffmanNode* decode(unsigned int bit, unsigned char* c);

private:
  unsigned char symbol_;
  int frequency_;
  HuffmanNode* left_;
  HuffmanNode* right_;
};


class CompareFrequency {
public:
  bool operator() (const HuffmanNode& lhs, const HuffmanNode& rhs) {
    return lhs.frequency() > rhs.frequency();
  }

  bool operator() (const HuffmanNode* lhs, const HuffmanNode* rhs) {
    return (*this)(*lhs, *rhs);
  }
};


void HuffmanNode::encode(CodedSymbol coded_symbol, CodingTable* encoding) {
  if (!left_) {
    assert(!right_);
    encoding->set(symbol_, coded_symbol);
  } else {
    CodedSymbol left_symbol = coded_symbol;
    left_symbol.push_bit(0);
    left_->encode(left_symbol, encoding);
    CodedSymbol right_symbol = coded_symbol;
    right_symbol.push_bit(1);
    right_->encode(right_symbol, encoding);
  }
}


HuffmanNode* HuffmanNode::decode(unsigned int bit, unsigned char* c) {
  assert(left_ && right_);
  HuffmanNode* next_node = bit ? right_ : left_;
  if (next_node->left_) {
    assert(next_node->right_);
    return next_node;
  }
  *c = next_node->symbol_;
  return NULL;
}


void HuffmanNode::dump (int level, ostream& out) const {
  for (int i = 0; i < level; ++i) {
    out << ((i % 4) ? "." : "+");
  }
  out << " ";
  out << showbase << hex << this << dec << " " ;
  if (left_ == NULL) {
    assert(right_ == NULL);
    out << "   " << setw(4) << showbase << hex << static_cast<int>(symbol_) << dec;
    out << " (";
    out.put(isprint(symbol_) ? symbol_ : ' ');
    out << ") ";
  } else {
    assert(right_ != NULL);
    out << showbase << hex;
    out << "left(" << left_ << ") right(" << right_ << ") ";
    out << dec;
    out << " " ;
  }
  out << frequency_;
  out << endl;
  if (left_) {
    left_->dump(level + 1, out);
    right_->dump(level + 1, out);
  }
}


HuffmanTree::HuffmanTree(const FrequencyTable& frequencies) {
  priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, CompareFrequency> nodes;

  for (int c = 0; c <= UCHAR_MAX; ++c) {
    if (frequencies.get(c) != 0) {
      HuffmanNode* node = new HuffmanNode(c, frequencies.get(c));
      nodes.push(node);
    }
  }

  while (true) {
    assert(!nodes.empty());
    HuffmanNode* right = nodes.top();
    nodes.pop();
    if (nodes.empty()) {
      root_ = right;
      break;
    }
    HuffmanNode* left = nodes.top();
    nodes.pop();
    HuffmanNode* parent = new HuffmanNode(left, right);
    nodes.push(parent);
  }
}


void HuffmanTree::encode(CodingTable* encoding) {
  assert(root_);
  assert(encoding);
  root_->encode(CodedSymbol(), encoding);
}


HuffmanNode* HuffmanTree::decode(unsigned int bit, unsigned char* c, HuffmanNode* node) {
  if (node == NULL) {
    node = root_;
  }
  return node->decode(bit, c);
}


void HuffmanTree::dump(ostream& out) const {
  if (root_ == NULL) {
    out << "Empty tree." << endl;
  } else {
    root_->dump(0, out);
  }
}
