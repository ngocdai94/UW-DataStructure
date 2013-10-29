// block.h
//
// Copyright 2013 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)

#include "block.h"

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iomanip>

#include "block_manager.h"

using namespace std;


const xuint32_t Block::NULL_BLOCK = static_cast<xuint32_t>(-1);
const xuint16_t Block::MAX_WORD_LEN = BLOCK_SIZE / 4;

BlockManager*  Block::manager_ = NULL;


Block::Block(xuint32_t blocknum) {
  *block_number() = blocknum;
  *item_count() = 0;
  *string_table() = BLOCK_SIZE;
  *left_child() = NULL_BLOCK;
}


int Block::initialize(const string& word, xuint32_t count, xuint32_t left, xuint32_t right) {
  *item_count() = 0;
  *left_child() = left;
  Item* item = set_item(0, word, count, right);
  if (!item) {
    cerr << "error initializing block "
         << showbase << hex << this << dec
         << " with word " << word << "\n";
    return -1;
  }
  manager_->update(this);
  return 0;
}


int Block::add_word(const string& word, Block::Promotion* promotion) {
  cerr << "add_word(): adding \"" << word << "\" to " << this
       << " (" << *block_number() << ")\n";

  // Look for the word (or insert point) using linear scan.  We could
  // use a binary search, but this is simpler to code *and* we have to
  // do a linear scan to shift the entries to make room anyway, so it
  // doesn't matter asymptotically.
  Item* item;
  xuint16_t insert_point = 0;
  for (item = begin_item(); item != end_item(); ++item) {
    if (word == string_at(item->offset)) {
      // Word found.  Yay.
      cerr << " word found at " << item << "(" << item - begin_item() << ")\n";
      ++item->count;
      update_and_release(this);
      return 0;
    }
    if (word < string_at(item->offset)) {
      // Word is (lexicographically) smaller than the current word in
      // the table.  Since this is the first time that happens
      // (because we immediately break out of the loop), we've found
      // the point in the table where we wish to insert the new word.
      break;
    }
    ++insert_point;
  }

  if (is_leaf()) {
    cerr << "insert_word(): inserting " << word
         << " into position " <<  item - begin_item() << "\n";
    if (has_space_for(word)) {
      // META: the comment "There is room to add word into this node."
      // is superfluous because it repeats what is said in the
      // conditional.
      return shift_items(item, word, 1, NULL_BLOCK, NULL_BLOCK);
    } else {
      return insert_split(insert_point, word, 1, NULL_BLOCK, NULL_BLOCK, promotion);
    }
  } else {
    // META: might put the comment "Not a leaf" or "Internal node"
    // here if the then part is long and the condition has scrolled
    // off the screen.  Judgement call.
    Block* child = get((item - 1)->right_child);
    Promotion child_promotion;
    xuint32_t child_block_number = *child->block_number();
    int result = child->add_word(word, &child_promotion);
    if (result <= 0) {
      release(this);
      return result;
    }
    if (has_space_for(child_promotion.pivot_word)) {
      return shift_items(item,
                         child_promotion.pivot_word,
                         child_promotion.count,
                         child_block_number,
                         child_promotion.new_sibling);
    } else {
      return insert_split(insert_point, child_promotion.pivot_word, child_promotion.count,
                          child_block_number, child_promotion.new_sibling, promotion);
    }
  }
}


void Block::print_words(ostream& out) {
  xuint32_t left = *left_child();
  if (left != NULL_BLOCK) {
    Block* left_block = get(left);
    left_block->print_words(out);
    release(left_block);
  }
  for (Item* item = begin_item(); item != end_item(); ++item) {
    out << setw(7) << item->count
         << " "
         << string_at(item->offset)
         << endl;
    xuint32_t right = item->right_child;
    // META: check for not non-null right child instead of calling
    // is_leaf() because this will work correctly even if the 2-3 tree
    // is mal-formed.  Robust debugging routines are a Good Thing.
    if (right != NULL_BLOCK) {
      Block* right_block = get(right);
      right_block->print_words(out);
      release(right_block);
    }
  }
}


Block* Block::allocate() {
  return manager_->allocate();
}


Block* Block::get(xuint32_t blocknum) {
  return manager_->get(blocknum);
}


void Block::update(Block* block) {
  manager_->update(block);
}


void Block::release(Block* block) {
  manager_->release(block);
}


int Block::block_count() {
  return manager_->block_count();
}


void Block::dump(ostream& out, int depth, bool recursive) {
  string indent(depth * 4, ' ');
  out << showbase;
  out << indent << "Block: " << this << " (depth = " << depth << ")\n";
  out << indent << "    block number: " << *block_number() << "\n";
  out << indent << "    number of items: " << *item_count() << "\n";
  out << indent << "    end of item table: " << free_offset() << "\n";
  out << indent << "    offset to string table: " << *string_table() << "\n";
  out << indent << "    free space: " << free_space() << "\n";
  out << indent << "    leftmost child: " << *left_child() <<  " (" << hex << *left_child() << dec << ")\n";
  if (*left_child() != NULL_BLOCK && recursive) {
    Block* child = get(*left_child());
    child->dump(out, depth + 1);
    release(child);
  }
  for (Item *it = begin_item(); it != end_item(); ++it) {
    out << indent << "    * " << string_at(it->offset) <<  " (" << it->count << ")\n";
    out << indent << "    right-child: " << it->right_child << " (" << hex << it->right_child << dec << ")\n";
    if (it->right_child != NULL_BLOCK && recursive) {
      Block* child = get(it->right_child);
      child->dump(out, depth + 1);
      release(child);
    }
  }
}


void Block::copy_items(xuint16_t from, xuint16_t to, Block* target) {
  Item* dst= target->end_item();
  for (Item* src = begin_item() + from; src != begin_item() + to; ++src) {
    char* s = string_at(src->offset);
    target->allocate_string(s);
    dst->offset = *(target->string_table());
    dst->count = src->count;
    dst->right_child = src->right_child;
    ++(*target->item_count());
    ++dst;
  }
}


void Block::allocate_string(const char *s) {
  // META: add one for the NUL byte.  YMMV whether this deserves a
  // comment, since this is basic knowlege of the language.
  xuint16_t len = strlen(s)+ 1;
  // Add the word to the top of the free space.
  *string_table() -= len;
  strcpy(string_at(*string_table()), s);
}


int Block::shift_items(Item* item, const string& word, xuint16_t count, xuint32_t left, xuint32_t right) {
  cerr << "shift_items(): adding \"" << word << "\" into node " << block_number()
       << " at position " << item - begin_item() << "\n";

  // Everything from item to the end of the table
  // needs to be shifted to make room for the item we're adding
  // here.  This is easier if we start at the end and work backwards.
  //
  for (Item* it = end_item(); it != item; --it) {
        *it = *(it - 1);
      }

  allocate_string(word.c_str());
  item->offset = *string_table();
  item->count = count;
  item->right_child = right;
  (item - 1)->right_child = left;
  ++*item_count();

  update_and_release(this);
  return 0;
}


xuint16_t Block::find_splitpoint(xuint16_t insert_point, const string& word) {
#if 0
  // Ideally we want to split at the midpoint so half the keys go into
  // each node.  Unfortunately, this has a pathological (buggy) case
  // if there are very long words and very short words.  The "half"
  // with the long words, plus the new word might overflow its block.
  xuint16_t midpoint = *item_count() / 2;
  return midpoint;
#elif 1
  // Split at the point that minimizes the difference in space used by
  // the left and right blocks.
  //
  // The splitpoint will be in the range 1..n-1 to ensure that there
  // is a word on the left and right.  The word at the split point
  // will be promoted to the parent node.
  //
  // Start at word 0.  The left block will be empty and the right
  // block will be the total string size plus the size of the inserted
  // word, minus the 0th word (which would be promoted).  Special case
  // would be the word being inserted at position 0, in which case,
  // the right block is just the string table and the split is the
  // inserted word.  Note that that this is an initialization to set
  // up the loop invariant.  The split point will never be 0.
  int left_size = 0;
  int right_size = BLOCK_SIZE - *string_table();
  xuint16_t word_index = 0;
  if (insert_point != 0) {
    right_size += word.size() + 1;
    right_size -= strlen(string_at(begin_item()[0].offset)) + 1;
    word_index = 1;
  }
  int min_delta = right_size;
  xuint16_t splitpoint = 0;

  // Loop: walk through the strings, incrementing the size of the left
  // block by the candidate splitpoint word and decrementing the right
  // block by the next word, which becomes the new candidate.
  //
  // Keep a separate counter for the candidate index and the word
  // index because we have to account for the additional insert word.
  int candidate;
  for (candidate = 1; candidate < *item_count() - 1; ++candidate) {
    if (candidate == insert_point) {
      left_size += word.size() + 1;
      right_size -= strlen(string_at(begin_item()[word_index].offset)) + 1;
    } else if (candidate + 1 == insert_point) {
      left_size +=  strlen(string_at(begin_item()[word_index].offset)) + 1;
      right_size -= word.size() + 1;
      ++word_index;
    } else {
      left_size += strlen(string_at(begin_item()[word_index].offset)) + 1;
      ++word_index;
      right_size -= strlen(string_at(begin_item()[word_index].offset)) + 1;
    }
    if (abs(right_size - left_size) < min_delta) {
      min_delta = abs(right_size - left_size);
      splitpoint = candidate;
    }
  }
  // Check for special case: the insert point is at the right end, so
  // the splitpoint might be at position n-1.
  //
  // This is overkill because we know that we can fit at least 3 words
  // into a block so we will always have a valid split by the time we
  // get to this point, even if it's less optimal.
  if (insert_point == *item_count()) {
    left_size += strlen(string_at(begin_item()[candidate - 1].offset)) + 1;
    right_size = word.size() + 1;
    if (abs(right_size - left_size) < min_delta) {
      min_delta = abs(right_size - left_size);
      splitpoint = candidate;
    }
  }

  assert(splitpoint != 0);
  return splitpoint;
#else
  // Alternate, less memory-efficient, but more readable (?) strategy
  // for finding the split point: keep a vector of the words and
  // calculate sizes by walking the vector.
  int left_size = 0;
  int right_size = 0;
  vector<int> word_sizes;

  Item* p = begin_item();
  for (int i = 0; i < insert_point; ++i) {
    int word_size = strlen(string_at(p[i].offset)) + 1;
    word_sizes.push_back(word_size);
    right_size += word_size;
  }
  word_sizes.push_back(word.size() + 1);
  right_size += word.size() + 1;
  for (int i = insert_point; i < *item_count(); ++i) {
    int word_size = strlen(string_at(p[i].offset)) + 1;
    word_sizes.push_back(word_size);
    right_size += word_size;
  }
  right_size -= word_sizes[0];

  int min_delta = right_size;
  xuint16_t splitpoint = 0;
  for (int i = 1; i < word_sizes.size() - 1; ++i) {
    left_size += word_sizes[i - 1];
    right_size -= word_sizes[i];
    if (abs(right_size - left_size) < min_delta) {
      min_delta = abs(right_size - left_size);
      splitpoint = i;
    }
  }
  return splitpoint;
#endif
}


int Block::insert_split(xuint16_t insert_point, const string& word,
                         xuint16_t count, xuint32_t left, xuint32_t right,
                         Block::Promotion* promotion) {
  cerr << "insert_split(): adding \"" << word << "\" into node " << block_number()
       << " at position " << insert_point << "\n";

  // Copy this into buffer so we can compact the string table when we
  // copy the items back after splitting.
  Block buffer(*this);

  Block* new_block = manager_->allocate();
  promotion->new_sibling = *new_block->block_number();

  // Splitpoint is the index of the item that will be the key
  // separating the two blocks (original plus new sibling), but the
  // item at splitpoint should be moved to the new sibling block if
  // the word being inserted is to be inserted at the midpoint (in
  // which case, the  insert word is the new pivot).
  xuint16_t splitpoint = find_splitpoint(insert_point, word);

  *(new_block->left_child()) = begin_item()[splitpoint].right_child;
  copy_items(splitpoint + (insert_point != splitpoint), *item_count(), new_block);

  // Reset the original (this) block and copy items back from the
  // buffer, which will compact the string table.
  *string_table() = BLOCK_SIZE;
  *item_count() = 0;
  buffer.copy_items(0, splitpoint, this);

  if (insert_point == splitpoint) {
    cerr << "insert_split(): word is pivot\n";
    promotion->pivot_word = word;
    promotion->count = count;
    *new_block->left_child() = right;
    begin_item()[splitpoint - 1].right_child = left;
    update_and_release(new_block);
    update_and_release(this);
  } else {
    Item* pivot = buffer.begin_item() + splitpoint;
    promotion->pivot_word = buffer.string_at(pivot->offset);
    promotion->count = pivot->count;
    cerr << "insert_split(): pivot word set to \"" << promotion->pivot_word << "\"\n";
    if (insert_point < splitpoint) {
      cerr << "insert_split(): inserting word into original node\n";
      shift_items(begin_item() + insert_point, word, count, left, right);
      update_and_release(new_block);
    } else {
      cerr << "insert_split(): inserting word into new sibling\n";
      insert_point -= splitpoint + 1;
      new_block->shift_items(new_block->begin_item() + insert_point,
                             word, count, left, right);
      update_and_release(this);
    }
  }

  return 1;
}


Block::Item*  Block::set_item(xuint16_t n, const string& word, xuint32_t count, xuint32_t right) {
  allocate_string(word.c_str());
  Item* item = begin_item() + n;
  item->offset = *string_table();
  item->count = count;
  item->right_child = right;
  ++*item_count();
  return item;
}


// Stand-alone dump function because gdb doesn't like calling methods
// from the prompt.
// META: this function may be called from the debugger, especially
// from a watchpoint.
int block_dump(Block* block) {
  block->dump(cerr, 0, false);
  return 0;
}
