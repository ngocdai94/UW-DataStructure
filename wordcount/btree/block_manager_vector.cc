// block_manager.cc
//
// Copyright 2013 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)

#include "block_manager.h"

#if BLOCK_MANAGER_IMPL != BLOCK_MANAGER_VECTOR
#error "BLOCK_MANAGER_IMPL must be #defined as BLOCK_MANAGER_VECTOR"
#endif


BlockManager::BlockManager() {
}


Block* BlockManager::allocate() {
  xuint32_t blocknum = blocks.size();
  Block* block = new Block(blocknum);
  blocks.push_back(block);
  return block;
}


Block* BlockManager::get(xuint32_t blocknum) {
  return blocks[blocknum];
}

void BlockManager::update(const Block* block) {
  // No-op for vector-based blocks.
}

void BlockManager::release(const Block* block) {
  // No-op for vector-based blocks.
  // META: This will mask two classes of bugs: releasing too soon,
  // leaving dangling pointer(s) and not releasing, leaking memory.
}

int BlockManager::block_count() {
  return blocks.size();
}
