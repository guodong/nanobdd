#pragma once

#include <atomic>
#include <stdint.h>

namespace nanobdd {

struct Node {
  Node()
      : refCount(0), level(0), low(nullptr), high(nullptr), inUse(false),
        nextFree(nullptr) {}

  Node(uint32_t _level, Node *_low, Node *_high)
      : refCount(0), level(_level), low(_low), high(_high), inUse(false),
        nextFree(nullptr) {}

  // refCount is a standalone counter used for GC eligibility only.
  inline void ref() { refCount.fetch_add(1, std::memory_order_seq_cst); }

  inline void deref() { refCount.fetch_sub(1, std::memory_order_seq_cst); }

  void markRec() {
    if (!inUse) {
      inUse = true;
      if (low != nullptr) {
        low->markRec();
      }
      if (high != nullptr) {
        high->markRec();
      }
    }
  }

  void unmark() {
    if (inUse) {
      inUse = false;
      return;
    }
  }

  std::atomic_uint32_t refCount;
  uint32_t level;
  Node *low{nullptr};
  Node *high{nullptr};
  bool inUse;
  // Used only by the allocator's free list; ignored by BDD logic.
  Node *nextFree{nullptr};
};

} // namespace nanobdd
