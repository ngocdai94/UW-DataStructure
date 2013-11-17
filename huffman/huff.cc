// Copyright 2012 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>

#include "bitstream.h"
#include "huffman_tree.h"

using namespace std;


void huff(const string& input_name) {
  ifstream in(input_name.c_str());
  FrequencyTable ft;
  unsigned int n = ft.count(in);
  HuffmanTree tree(ft);
  tree.dump(cerr);
  CodingTable encoding;
  tree.encode(&encoding);
  encoding.dump();

  string output_name = input_name + ".huff";
  ofstream out(output_name.c_str());
  ft.write(out);
  out.write(reinterpret_cast<const char *>(&n), sizeof(int));

  in.clear();
  in.seekg(0);
  OutputBits bits(&out);
  int c;
  while ((c = in.get()) != EOF) {
    bits.emit(encoding.get(static_cast<unsigned char>(c)));
  }
  bits.flush();
}


int main(int argc, char* argv[]) {
  if (argc != 2) {
    cerr << "usage: " << argv[0] << " _filename_" << endl;
    exit(EXIT_FAILURE);
  }

  huff(argv[1]);
  return (EXIT_SUCCESS);
}
