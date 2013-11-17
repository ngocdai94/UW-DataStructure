// Copyright 2012 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)
//

#ifndef _BITSTREAM_H_
#define _BITSTREAM_H_

#include <fstream>

#include "coding_table.h"


class InputBits {
public:
  InputBits(std::istream* in) : in_(in), buf_count_(0), buf_bits_(0) {}

  unsigned int get();

private:
  std::istream* in_;
  unsigned int buf_count_;
  unsigned char buf_bits_;
};


class OutputBits {
public:
  OutputBits(std::ofstream* out) : out_(out), buf_count_(0), buf_bits_(0) {}

  void emit(const CodedSymbol& symbol);

  void flush();

  std::ofstream* out() {return out_;}

private:
  std::ofstream* out_;
  int buf_count_;
  unsigned char buf_bits_;
};


#endif
