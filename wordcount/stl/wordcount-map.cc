// wordcount.cc
//
// Copyright 2013 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)

#include <iostream>
#include <map>

using namespace std;

int main() {
  map<string, int> count;
  string word;
  while(cin >> word) {
    ++count[word];
  }
  for (map<string, int>::const_iterator it = count.begin(); it != count.end(); ++it) {
    cout << it->second << "  " << it->first << endl;
  }
  return 0;
}
