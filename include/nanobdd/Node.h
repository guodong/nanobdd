#pragma once

#include <stdint.h>
#include <atomic>
#include <memory>

namespace nanobdd {

struct Node {
  Node() : refCount(std::make_unique<std::atomic_uint32_t>(0)), inUse(false) {}

  Node(uint32_t _level, Node* _low, Node* _high)
      : level(_level),
        low(_low),
        high(_high),
        refCount(std::make_unique<std::atomic_uint32_t>(0)) {}

  inline void
  ref() {
    (*refCount)++;
  }

  inline void
  deref() {
    (*refCount)--;
  }

  void
  mark() {
    if (inUse) {
      return;
    } else if (refCount->load() != 0) {
      inUse = true;
      if (low != nullptr) {
        low->mark();
      }
      if (high != nullptr) {
        high->mark();
      }
    }
  }

  void
  unmark() {
    if (!inUse) {
      return;
    } else {
      inUse = false;
      if (low != nullptr) {
        low->unmark();
      }
      if (high != nullptr) {
        high->unmark();
      }
    }
  }

  std::unique_ptr<std::atomic_uint32_t> refCount;
  uint32_t level;
  Node* low{nullptr};
  Node* high{nullptr};
  bool inUse;
};

} // namespace nanobdd