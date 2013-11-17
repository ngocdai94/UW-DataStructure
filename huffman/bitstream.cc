// bitstream.cc
//
// Copyright 2012 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)

#include "bitstream.h"

#include <climits>

using namespace std;


unsigned int InputBits::get() {
  if (buf_count_ == 0) {
    char c;
    in_->get(c);
    buf_bits_ = static_cast<unsigned char>(c);
    buf_count_ = CHAR_BIT;
  }
  unsigned int bit = (buf_bits_ >> --buf_count_) & 1;
  return bit;
}


void OutputBits::emit(const CodedSymbol& symbol) {
  Bits bits = symbol.encoding();
  for (int num_bits = symbol.num_bits() - 1; num_bits >= 0; --num_bits) {
    unsigned bit = (bits >> num_bits) & 1;
    buf_bits_ = (buf_bits_ << 1) | bit;
    ++buf_count_;
    if (buf_count_ == CHAR_BIT) {
      out_->put(buf_bits_);
      buf_bits_ = 0;
      buf_count_ = 0;
    }
  }
}


void OutputBits::flush() {
  if (buf_count_ != 0) {
    buf_bits_ = buf_bits_ << (CHAR_BIT - buf_count_);
    out_->put(buf_bits_);
  }
}
