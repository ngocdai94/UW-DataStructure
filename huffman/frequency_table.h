// Copyright 2012 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)
//

#ifndef FREQUENCY_TABLE_H_
#define FREQUENCY_TABLE_H_

#include <climits>
#include <fstream>
#include <iostream>
#include <istream>
#include <ostream>


#if defined(LARGE_INPUT)
typedef unsigned int Frequency;
#else
typedef unsigned short Frequency;
#endif


class FrequencyTable {
public:
  FrequencyTable();
  explicit FrequencyTable(std::istream& in);

  unsigned int count(std::istream& text);
  void read(std::istream& in);
  void write(std::ostream& out);

  Frequency get(unsigned char c) const {return data_[c];}

  void dump_frequency(unsigned char c, std::ostream& out) const;
  void dump(std::ostream& out=std::cerr) const;

private:
  void init();

  Frequency data_[UCHAR_MAX + 1];
};


#endif
