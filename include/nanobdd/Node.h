#pragma once

#include <atomic>
#include <memory>
#include <stdint.h>

namespace nanobdd {

struct Node {
  Node() : refCount(std::make_unique<std::atomic_uint32_t>(0)), inUse(false) {}

  Node(uint32_t _level, Node *_low, Node *_high)
      : level(_level), low(_low), high(_high),
        refCount(std::make_unique<std::atomic_uint32_t>(0)) {}

  inline void ref() { (*refCount)++; }

  inline void deref() { (*refCount)--; }

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

  std::unique_ptr<std::atomic_uint32_t> refCount;
  uint32_t level;
  Node *low{nullptr};
  Node *high{nullptr};
  bool inUse;
  // Used only by the allocator's free list; ignored by BDD logic.
  Node *nextFree{nullptr};
};

} // namespace nanobdd
