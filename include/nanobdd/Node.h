#pragma once

#include <atomic>
#include <memory>
#include <stdint.h>

namespace nanobdd {

struct Node {
  Node() : refCount(std::make_unique<std::atomic_uint32_t>(0)) {}
  void ref() { (*refCount)++;}
  void deref() { (*refCount)--;}
  std::unique_ptr<std::atomic_uint32_t> refCount;
  // std::atomic_uint32_t refCount{0};
  // uint32_t refCount{0};
  uint32_t level;
  Node* low;
  Node* high;
};

} // namespace nanobdd