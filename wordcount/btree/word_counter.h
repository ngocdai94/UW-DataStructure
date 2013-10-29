// word_counter.h
//
// Copyright 2013 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)

#if !defined(WORD_COUNTER_H_)
#define WORD_COUNTER_H_

#include <iostream>
#include <string>

// TODO: we really want the types from <cstdin> in C++11, but for now,
// we'll import from future.
#include "future.h"


class WordCounter {
 public:
  // META: this does not need a commment.  It's a constructor; you can
  // see that it's a constructor.  Adding a comment that says it's a
  // constructor doesn't add anything.  Of course, if you're using a
  // system like Javadoc that requires comments, you'll write useless
  // boilerplate comments.
  WordCounter();

  // Add word to the table, or increment its count if already
  // present.
  //
  // Returns 0 on success; -1 if word cannot be added.
  int add_word(const std::string& word);

  // Print the words in sorted order preceeded by count (should be
  // compatible with output from the "uniq -c" shell command).
  void print_words(std::ostream& out=std::cout);

  void dump(std::ostream& out=std::cerr);

 private:
  // "Root" of the BTree structure. The 32-bit unsigned integer is
  // used as a handle.
  xuint32_t root_;
};


#endif
