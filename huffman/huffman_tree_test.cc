// Copyright 2012 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)
//

#include <iostream>

#include "frequency_table.h"
#include "huffman_tree.h"

using namespace std;


int main() {
  FrequencyTable ft;
  unsigned int n = ft.count(cin);
  cerr << "Total bytes read: " << n << endl;
  HuffmanTree tree(ft);
  tree.dump();
  CodingTable encoding;
  tree.encode(&encoding);
  encoding.dump();
}
