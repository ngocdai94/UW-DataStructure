// Copyright 2012 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)
//

#ifndef CODING_TABLE_H_
#define CODING_TABLE_H_

#include <climits>
#include <iostream>
#include <ostream>


typedef unsigned int Bits;


class CodedSymbol {
public:
  CodedSymbol(int num_bits=0, Bits encoding=0) : num_bits_(num_bits), encoding_(encoding) {}

  int num_bits() const {return num_bits_;}
  Bits encoding() const {return encoding_;}

  void push_bit(bool bit) {
    encoding_ = encoding_ << 1 | (bit & 1);
    ++num_bits_;
  }

  void dump(unsigned char c, std::ostream& out=std::cerr) const;

private:
  int num_bits_;
  Bits encoding_;
};


class CodingTable {
public:
  CodingTable();

  void set(unsigned char c, const CodedSymbol& symbol) {
    data_[c] = symbol;
  }

  CodedSymbol get(unsigned char c) {return data_[c];}

  void dump(std::ostream& out=std::cerr) const;

private:
  CodedSymbol data_[UCHAR_MAX + 1];
};


#endif
