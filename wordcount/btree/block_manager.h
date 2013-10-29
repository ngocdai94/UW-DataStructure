// block_manager.h
//
// Copyright 2013 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)

#if !defined(BLOCK_MANAGER_H_)
#define BLOCK_MANAGER_H_

#include <vector>

#include "block.h"
#include "future.h"


// META: preprocessor magic.  The command-line flag
// -DBLOCK_MANAGER_IMPL=BLOCK_MANAGER_FILE overrides the default
// behavior (BLOCK_MANAGER_VECTOR).

#define BLOCK_MANAGER_VECTOR 1
#define BLOCK_MANAGER_FILE   2

#if !defined BLOCK_MANAGER_IMPL
#define BLOCK_MANAGER_IMPL BLOCK_MANAGER_VECTOR
#endif


// Block Manager: proxy for the file system.
class BlockManager {
 public:
  BlockManager();

  // Allocate a new block.
  Block* allocate();

  // Return pointer to existing block with given block number
  Block* get(xuint32_t blocknum);

  void update(const Block* block);

  void release(const Block* block);

  int block_count();

 private:
  // META: Implementation details that leak into the interface.  A
  // better way to do this might be to make the class abstract and
  // define concrete subclasses to implement the chosen policy.
#if BLOCK_MANAGER_IMPL == BLOCK_MANAGER_VECTOR
  // Proxy for file.
  std::vector<Block*> blocks;
#elif BLOCK_MANAGER_IMPL == BLOCK_MANAGER_FILE
  int fd_;
  xuint32_t blocks_allocated_;
#else
#error "BLOCK_MANAGER_IMPL must be defined as valid implementation"
#endif

  // Disallow copy and assign.
  BlockManager(const BlockManager&);
  void operator=(const BlockManager&);
};


#endif
