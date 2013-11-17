// Copyright 2012 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)
//

#include "coding_table.h"

#include <iomanip>

using namespace std;


void CodedSymbol::dump(unsigned char c, ostream& out) const {
  if (num_bits_ > 0) {
    out << "   " << setw(4) << showbase << hex << static_cast<int>(c) << dec;
    out << " (";
    out.put((isprint(c) ? c : ' '));
    out << "): [";
    for (int i = num_bits_ - 1 ; i >= 0; --i) {
      out << ((encoding_ & (1 << i)) ? "1" : "0");
    }
    out << "] ";
    out << (num_bits_);
    out << endl;
  }
}


CodingTable::CodingTable() {
  for (int i = 0 ; i <= UCHAR_MAX; ++i) {
    data_[i] = CodedSymbol(0, 0);
  }
}

void CodingTable::dump(ostream& out) const {
  for (int i = 0; i <= UCHAR_MAX; ++i) {
    data_[i].dump(static_cast<unsigned char>(i), out);
  }
}
