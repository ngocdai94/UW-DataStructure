// block.h
//
// Copyright 2013 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)

#if !defined(BLOCK_H_)
#define BLOCK_H_

#include <iostream>
#include <string>

#include "future.h"


// META: this idiom allows the possibility of overriding the variable
// using a compile-time command-line flag, e.g.: -DBLOCK_SIZE=4096
// META: the commented-out #define is poor style, but something that
// one tends to do during development.
#if !defined(BLOCK_SIZE)
#define BLOCK_SIZE 128
//#define BLOCK_SIZE 512
#endif


// META: this is a forward declaration because Block uses a Block
// manager pointer, but BlockManager needs to know about Blocks.  The
// forward declaration allows us to break the circularity of the
// definition.
class BlockManager;


// Block: untyped blob of data of size BLOCK_SIZE.  The type
// represents a BTree node.  Special accessor functions perform the
// appropriate casting operations.
//
// META: note that this declaration appears in the .cc file and not
// the .h because it's a private implementation detail of the
// WordCounter class
class Block {
 public:
  // Promotion: data items that must be passed back to the caller
  // during insertion in case a node overflows and must be split.
  //
  // META: Nested struct.  Again, a way of shrouding implementation
  // details.
  struct Promotion {
    std::string pivot_word;
    xuint16_t count;
    xuint32_t new_sibling;
  };

  static const xuint32_t NULL_BLOCK;
  static const xuint16_t MAX_WORD_LEN;

  Block(xuint32_t blocknum=0);

  // Initialize me.  Do not initialize in the constructor because the
  // object is allocated by a factory object and allocation and
  // initialization are separate functions.
  int initialize(const std::string& word, xuint32_t count, xuint32_t left, xuint32_t right);

  // If word is already in the list, increment the count.
  // Otherwise, insert the word into the block and split the block
  // if an overflow occurs.
  //
  // Returns:
  //   0 if word is present or inserted okay
  //   1 if the block is split
  //   -1 on error
  int add_word(const std::string& word, Promotion* promotion);

  // Print the counts and words of the block.
  void print_words(std::ostream& out=std::cout);

  xuint32_t get_block_number() const {return *const_cast<Block*>(this)->block_number();}

  static void set_manager(BlockManager* manager) {
    manager_ = manager;
  }

  // Dump routine for debugging.  Defaults to printing the entire
  // subtree from given block.  Single block only is printed when
  // called with recursive=false.
  // META: this function is particularly useful when called from the
  // debugger.
  void dump(std::ostream& out=std::cerr, int depth=0, bool recursive=true);

 private:
  friend class BlockManager;
  friend class WordCounter;

  struct Item {
    xuint16_t offset;
    xuint16_t count;
    xuint32_t right_child;
  };

  // META: mangager_ is statically intialized to NULL and must be
  // created prior to its first use.
  // META: static definition is a code smell: it means we can't use
  // more than one block manager in the program.  A better approach
  // would be to to make this a template parameter.
  static BlockManager* manager_;

  char arena_[BLOCK_SIZE];

  // Pseudo fields:
  // block_number
  // item_count (number of entries in block)
  // string_table (offset to first location past the free space)
  // left_child (blocknumber of leftmost child or -1)
  // items starting from begin_item up to but excluding end_item
  // free_offset (start location of free space)
  // string_at (string from given location)

  xuint32_t* block_number() {
    return reinterpret_cast<xuint32_t*>(this);
  }

  xuint16_t* item_count() {
    return reinterpret_cast<xuint16_t*>(block_number() + 1);
  }

  xuint16_t* string_table() {
    return item_count() + 1;
  }

  xuint32_t* left_child() {
    return reinterpret_cast<xuint32_t*>(string_table() + 1);
  }

  Item* begin_item() {
    return reinterpret_cast<Item*>(left_child() + 1);
  }

  Item* end_item() {
    return begin_item() + *item_count();
  }

  xuint16_t free_offset() {
    xuintptr_t base = reinterpret_cast<xuintptr_t>(this);
    xuintptr_t begin_free = reinterpret_cast<xuintptr_t>(end_item());
    return static_cast<xuint16_t>(begin_free - base);
  }

  xuint16_t free_space() {
    return *string_table() - free_offset();
  }

  char *string_at(xuint16_t offset) {
    return reinterpret_cast<char*>(this) + offset;
  }

  // Utility functions for insertion (add_word).

  void copy_items(xuint16_t from, xuint16_t to, Block* target);

  void allocate_string(const char* s);

  int shift_items(Item* item,
                  const std::string& word,
                  xuint16_t count,
                  xuint32_t left,
                  xuint32_t right);

  xuint16_t find_splitpoint(xuint16_t insert_point, const std::string& word);

  // Insert an entry into the node at insert_point, splitting the node
  // and passing the overflow to the parent.
  // Always returns 1 because this is the split case of add_word().
  int insert_split(xuint16_t insert_point, const std::string& word,
                   xuint16_t count, xuint32_t left, xuint32_t right,
                   Block::Promotion* promotion);

  // Delegation to the block manager
  //
  // META: These are really short functions but we can't inline them
  // here because the definition of BlockManager is not yet known.
  static Block* allocate();
  static Block* get(xuint32_t blocknum);
  static void update(Block* block);
  static void release(Block* block);
  static int block_count();

  // Helper functions

  static void update_and_release(Block* block) {
    update(block);
    release(block);
  }

  // Other utility functions

  bool is_leaf() {
    return *left_child() == NULL_BLOCK;
  }

  bool has_space_for(const std::string& word) {
    return word.size() +1 + sizeof(Item) <= free_space();
  }

  // Caller is responsible for ensuring sufficient space in block.
  Item* set_item(xuint16_t n, const std::string& word, xuint32_t count, xuint32_t right=NULL_BLOCK);
};


#endif
