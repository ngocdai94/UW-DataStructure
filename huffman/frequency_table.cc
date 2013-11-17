// Copyright 2012 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)
//

#include "frequency_table.h"

#include <cassert>
#include <cctype>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>

using namespace std;


FrequencyTable::FrequencyTable() {
  init();
}


FrequencyTable::FrequencyTable(std::istream& in) {
  init();
  read(in);
}


unsigned int FrequencyTable::count(istream& text) {
  int c;
  unsigned int total = 0;
  while ((c = text.get()) != EOF) {
    assert((c >= 0) && (c <= UCHAR_MAX));
#if !defined(LARGE_INPUT)
    if (data_[c] == USHRT_MAX) {
      cerr << "Input too big, reading character 0x"
           << hex << c;
      if (isprint(c)) {
        cerr << " (" << char(c) << ")";
      }
      cerr << endl;
      exit(EXIT_FAILURE);
    }
#endif
    ++data_[c];
    ++total;
  }
  return total;
}


void FrequencyTable::read(istream& in) {
  in.read(reinterpret_cast<char *>(data_), sizeof(data_));
  if (!in.good()) {
    cerr << "unknown error reading frequency table" << endl;
    exit(EXIT_FAILURE);
  }
}


void FrequencyTable::write(ostream& out) {
  cerr << "FrequencyTable: writing file\n";
  out.write(reinterpret_cast<const char *>(data_), sizeof(data_));
  cerr << "FrequencyTable: file written\n";
  if (!out.good()) {
    cerr << "unknown error writing frequency table" << endl;
    //exit(EXIT_FAILURE);
  }
}


void FrequencyTable::dump_frequency(unsigned char c, ostream& out) const {
  out << "   " << setw(4) << showbase << hex << static_cast<int>(c) << dec;
  out << " (";
  out.put((isprint(c) ? c : ' '));
  out << "): ";
  out << setw(7) << data_[c];
}


void FrequencyTable::dump(ostream& out) const {
  for (int c = 0; c <= UCHAR_MAX; ++c) {
    if (data_[c] != 0) {
      dump_frequency(c, out);
      out << endl;
    }
  }
}


void FrequencyTable::init() {
   memset(data_, 0, (UCHAR_MAX + 1) * sizeof(Frequency));
}
