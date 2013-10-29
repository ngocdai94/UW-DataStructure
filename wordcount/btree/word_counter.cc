// word_counter.cc
//
// Copyright 2013 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)

#include "word_counter.h"

#include <cassert>
#include <cstring>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <vector>

#include "block.h"
#include "block_manager.h"

using namespace std;


WordCounter::WordCounter():  root_(Block::NULL_BLOCK)
{
  // META: Bad Style Warning: don't allocate pointers in a
  // constructor.  There's no way to signal an error (allocation
  // failure) except by throwing exceptions, which forces client code
  // to use exceptions.
  BlockManager* manager = new(BlockManager);
  Block::set_manager(manager);
}


int WordCounter::add_word(const string& word) {
  if (word.length() + 1 > Block::MAX_WORD_LEN) {
    cerr << "word too long: " << word << endl;
    return -1;
  }

  if (root_ == Block::NULL_BLOCK) {
    // Degenerate case.
    Block* block = Block::allocate();
    root_ = *block->block_number();
    block->initialize(word, 1, Block::NULL_BLOCK, Block::NULL_BLOCK);
    Block::update_and_release(block);
    return 0;
  }

  Block* old_root_block = Block::get(root_);
  Block::Promotion promotion;
  int result = old_root_block->add_word(word, &promotion);
  if (result <= 0) {
    return result;
  }

  Block* new_root = Block::allocate();
  new_root->initialize(promotion.pivot_word, promotion.count, root_, promotion.new_sibling);
  root_ = *new_root->block_number();
  Block::update_and_release(new_root);
  return 0;
}


void WordCounter::print_words(ostream& out) {
  if (root_ != Block::NULL_BLOCK) {
    Block* root = Block::get(root_);
    root->print_words(out);
    Block::release(root);
  }
}


void WordCounter::dump(ostream& out) {
  Block* root = Block::get(root_);
  out << "block count: " << Block::block_count() << "\n";
  root->dump(out);
  Block::release(root);
}
