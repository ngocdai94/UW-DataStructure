// word_counter_main.cc
//
// Copyright 2013 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)


#include <iostream>
#include <string>

#include "word_counter.h"

using namespace std;


#if !defined(VERBOSE)
#define VERBOSE 1
#endif

int Verbose = VERBOSE;


int main() {
  string word;
  WordCounter counter;
  int n = 0;
  while (cin >> word) {
    if (Verbose > 0) {
      cerr << "Reading: \"" << word << "\" (" << ++n << ")\n";
    }
    int status = counter.add_word(word);
    if (status < 0) {
      cerr << "Error adding word " << word << endl;
      return 1;
    }
    if (Verbose > 0) {
      cerr << "\n";
      cerr << "after inserting: " << word << "\n";
      if (Verbose > 1) {
        counter.dump();
        counter.print_words(cerr);
      }
    }
  }
  if (Verbose > 0) {
    cerr << endl;
    cerr << "Final tree dump:" << endl;
    counter.dump();
  }
  counter.print_words(cout);
  return 0;
}
