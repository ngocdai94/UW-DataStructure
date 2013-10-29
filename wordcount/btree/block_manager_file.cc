// block_manager_file.cc
//
// Low-level IO implementation of block manager
//
// Copyright 2013 Systems Deployment, LLC
// Author: Morris Bernstein (morris@systems-deployment.com)

#include "block_manager.h"

#include <cstdlib>
#include <cstdio>
#include <fcntl.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "block.h"

using namespace std;

#if BLOCK_MANAGER_IMPL != BLOCK_MANAGER_FILE
#error "BLOCK_MANAGER_IMPL must be #defined as BLOCK_MANAGER_FILE"
#endif


BlockManager::BlockManager() {
  blocks_allocated_ = 0;
  fd_ = open("word_count.dat.tmp", O_RDWR|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);
  if (fd_ < 0) {
    perror("unable to open word_count.dat.tmp");
    exit(1);
  }
}


Block* BlockManager::allocate() {
  Block* new_block = new Block(blocks_allocated_++);
  cerr << "ALLOCATE new " << new_block->get_block_number() << ": " << new_block << endl;
  return new_block;
}

Block* BlockManager::get(xuint32_t blocknum) {
  off_t offset = lseek(fd_, blocknum * BLOCK_SIZE, SEEK_SET);
  if (offset == static_cast<off_t>(-1)) {
    perror("BlockManager::get(): unable to seek requested block");
    cerr << "requested block was " << blocknum << endl;
    exit(1);
  }
  Block* block = new Block();
  cerr << "ALLOCATE get " << blocknum << ": " << block << endl;
  if (read(fd_, reinterpret_cast<void*>(block), BLOCK_SIZE) != BLOCK_SIZE) {
    perror("unable to read block");
    cerr << "requested block was " << blocknum << endl;
    exit(1);
  }
  return block;
}

void BlockManager::update(const Block* block) {
  off_t offset = lseek(fd_, block->get_block_number() * BLOCK_SIZE, SEEK_SET);
  if (offset == static_cast<off_t>(-1)) {
    perror("BlockManager::update(): unable to seek requested block");
    cerr << "requested block was " << block->get_block_number() << endl;
    exit(1);
  }
  void* non_const_block = reinterpret_cast<void*>(const_cast<Block*>(block));
  if (write(fd_, non_const_block, BLOCK_SIZE) != BLOCK_SIZE) {
    perror("unable to write block");
    cerr << "requested block was " << block->get_block_number() << endl;
    exit(1);
  }
}

void BlockManager::release(const Block* block) {
  cerr << "DELETE: " << block << endl;
  delete block;
}

int BlockManager::block_count() {
  return blocks_allocated_;
}
