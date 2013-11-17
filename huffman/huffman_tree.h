// Copyright 2012 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)
//

#ifndef HUFFMAN_TREE_H_
#define HUFFMAN_TREE_H_

#include <cstdlib>
#include <iostream>
#include <ostream>

#include "coding_table.h"
#include "frequency_table.h"


class HuffmanNode;


class HuffmanTree {
public:
  HuffmanTree(const FrequencyTable& frequencies);

  void encode(CodingTable* encoding);

  HuffmanNode* decode(unsigned int bit,  unsigned char* c, HuffmanNode* node=NULL);

  void dump(std::ostream& out=std::cerr) const;

private:
  HuffmanNode* root_;
};


#endif
