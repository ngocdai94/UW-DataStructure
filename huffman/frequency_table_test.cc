// Copyright 2012 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)
//

#include <fstream>
#include <iostream>
#include <string>

#include "frequency_table.h"

using namespace std;


int main(int argc, char *argv[]) {
  if (argc != 2) {
    cerr << "usage: " << argv[0] << " _file_" << endl;
    return 1;
  }

  FrequencyTable ft1;
  ifstream data(const_cast<const char *>(argv[1]));
  ft1.count(data);
  cout << "Initial Count:" << endl;
  ft1.dump(cout);

  ofstream saved((string(argv[1]) + ".ft").c_str());
  ft1.write(saved);
  saved.close();
  ifstream recover((string(argv[1]) + ".ft").c_str());
  FrequencyTable check;
  check.read(recover);
  cout << "Final Count:" << endl;
  check.dump(cout);

  return 0;
}
