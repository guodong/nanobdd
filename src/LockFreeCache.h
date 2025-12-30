#pragma once

#include <Common.h>
#include <nanobdd/Node.h>
#include <vector>

namespace nanobdd {

struct LockFreeCacheEntry {
  Node* node{nullptr};
  Node* left{nullptr};
  Node* right{nullptr};
  uint32_t op{0};
  std::atomic_int useCount{0};
};

class LockFreeCache {
 public:
  LockFreeCache(size_t size) : size_(size), cache_(size) {}

  Node*
  lookup(uint32_t hash, Node* left, Node* right, Operator op) {
    Node* out = nullptr;
    auto& entry = cache_.at(hash);
    if (entry.node == nullptr) {
      return nullptr;
    }
    int uc = 0;
    while (!entry.useCount.compare_exchange_strong(
        uc, uc + 1, std::memory_order_acquire)) {
      if (uc == -1) {
        return nullptr;
      }
    }

    if (entry.op == op) {
      if ((isCommutativeOperator(op) &&
           ((entry.left == left && entry.right == right) ||
            (entry.left == right && entry.right == left))) ||
          (!isCommutativeOperator(op) && entry.left == left &&
           entry.right == right)) {
        // NOTE: bellow code may returns incorrect node, I guess it is caused by
        // CPU cache visibility. To solve that, assign it to local varible
        // `out`. entry.useCount--; return entry.node;
        out = entry.node;
      }
    }

    entry.useCount--;
    return out;
  }

  void
  insert(uint32_t hash, Node* node, Node* left, Node* right, uint32_t op) {
    auto& entry = cache_.at(hash);
    int uc = 0;

    if (entry.useCount.compare_exchange_weak(uc, -1)) {
      entry.node = node;
      entry.left = left;
      entry.right = right;
      entry.op = op;
      entry.useCount.store(0, std::memory_order_release);
    }
  }

  size_t size() const {
    return size_;
  }

  void
  invalidateAll() {
    for (auto& entry : cache_) {
      entry.node = nullptr;
      entry.left = nullptr;
      entry.right = nullptr;
      entry.useCount.store(0, std::memory_order_release);
    }
  }

 private:
  bool
  isCommutativeOperator(Operator op) {
    return op <= 3;
  }

  size_t size_;
  std::vector<LockFreeCacheEntry> cache_;
};

} // namespace nanobdd
