#pragma once

#include <stdint.h>
#include <atomic>
#include <memory>

namespace nanobdd {

struct Node {
  Node() : refCount(std::make_unique<std::atomic_uint32_t>(0)), inUse(false) {}

  inline void
  ref() {
    (*refCount)++;
  }

  inline void
  deref() {
    (*refCount)--;
  }

  std::unique_ptr<std::atomic_uint32_t> refCount;
  uint32_t level;
  Node* low;
  Node* high;
  bool inUse;
};

} // namespace nanobdd