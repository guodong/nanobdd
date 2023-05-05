#pragma once

#include <Node.h>
#include <shared_mutex>
#include <vector>

namespace nanobdd {

struct CacheEntry {
  Node* node;
  Node* left;
  Node* right;
  uint32_t op;
  std::unique_ptr<std::shared_mutex> mutex;
};

class Cache {
 public:
  Cache(size_t size) : size_(size), cache_(size) {
    for (auto& e : cache_) {
      e.mutex = std::make_unique<std::shared_mutex>();
    }
  }
  Cache(size_t size, bool opCache)
      : size_(size),
        cache_(size),
        andCache_(size),
        orCache_(size),
        notCache_(size),
        diffCache_(size) {
    for (auto& e : cache_) {
      e.mutex = std::make_unique<std::shared_mutex>();
    }
    for (auto& e : andCache_) {
      e.mutex = std::make_unique<std::shared_mutex>();
    }
    for (auto& e : orCache_) {
      e.mutex = std::make_unique<std::shared_mutex>();
    }
    for (auto& e : notCache_) {
      e.mutex = std::make_unique<std::shared_mutex>();
    }
    for (auto& e : diffCache_) {
      e.mutex = std::make_unique<std::shared_mutex>();
    }
  }

  Node*
  lookup(uint32_t hash, Node* left, Node* right, uint32_t op) {
    auto& cache = getCache(op);
    auto& entry = cache.at(hash);
    std::shared_lock<std::shared_mutex> lock(*entry.mutex);
    // if (entry.op == op && entry.left == left && entry.right == right) {
    //   return entry.node;
    // }
    if (op > 2 && entry.op == op && entry.left == left &&
        entry.right == right) {
      return entry.node;
    } else {
      // and/or are commutative.
      // if (entry.op == op &&
      //     ((entry.left == left && entry.right == right))) {
      if (entry.op == op &&
          ((entry.left == left && entry.right == right) ||
          (entry.left == right && entry.right == left))) {
        return entry.node;
      }
    }

    return nullptr;
  }

  void
  insert(uint32_t hash, Node* node, Node* left, Node* right, uint32_t op) {
    auto& cache = getCache(op);
    auto& entry = cache.at(hash);
    // const std::lock_guard<std::shared_mutex> lock(*entry.mutex);
    if ((*entry.mutex).try_lock()) {
      entry.node = node;
      entry.left = left;
      entry.right = right;
      entry.op = op;
      (*entry.mutex).unlock();
    }
  }

  const size_t size() {
    return size_;
  }

 private:
  std::vector<CacheEntry>&
  getCache(uint32_t op) {
    return cache_;
    if (op == 0) {
      return andCache_;
    } else if (op == 1) {
      return orCache_;
    } else if (op == 2) {
      return notCache_;
    } else {
      return diffCache_;
    }
  }
  size_t size_;
  std::vector<CacheEntry> cache_;
  std::vector<CacheEntry> andCache_;
  std::vector<CacheEntry> orCache_;
  std::vector<CacheEntry> notCache_;
  std::vector<CacheEntry> diffCache_;
};

} // namespace nanobdd