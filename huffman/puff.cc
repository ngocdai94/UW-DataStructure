// Copyright 2012 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)
//

#include <fstream>
#include <iostream>

#include "bitstream.h"
#include "coding_table.h"
#include "frequency_table.h"
#include "huffman_tree.h"

using namespace std;


void puff(const std::string& input_name) {
  ifstream in(input_name.c_str());
  string output_name = input_name + ".puff";
  ofstream out(output_name.c_str());
  FrequencyTable ft(in);
  HuffmanTree tree(ft);
  tree.dump(cerr);
  unsigned int n;
  in.read(reinterpret_cast<char *>(&n), sizeof(int));
  InputBits bits(&in);
  for (; n > 0; --n) {
    unsigned char c;
    HuffmanNode* node = NULL;
    do {
      node = tree.decode(bits.get(), &c, node);
    } while (node);
    out.put(c);
  }
}


int main(int argc, char* argv[]) {
  if (argc != 2) {
    cerr << "usage: " << argv[0] << " _filename_" << endl;
    exit(EXIT_FAILURE);
  }

  puff(argv[1]);
  return (EXIT_SUCCESS);
}
