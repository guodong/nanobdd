#pragma once

#include <nanobdd/Node.h>

#include <iostream>
#include <shared_mutex>
#include <utility>
#include <vector>

namespace nanobdd {

class Bucket {
public:
  Bucket() = default;

  Node *find(uint32_t level, Node *low, Node *high) const;

  // Returns (node, inserted?). If another thread inserted the same node first,
  // the existing pointer is returned and the caller should recycle its
  // candidate node.
  std::pair<Node *, bool> insertIfAbsent(uint32_t level, Node *low, Node *high,
                                         Node *candidate);

  size_t numNodes() const;

  // GC helpers (call while holding the table-wide exclusive lock).
  void markNodesUnsafe();
  size_t sweepNodesUnsafe(std::vector<Node *> &freed);
  void unmarkNodesUnsafe();

  void debugNodes() const;

  // Rehash helper: moves all nodes out of this bucket.
  void extractAllNodes(std::vector<Node *> &out);
  void insertExisting(Node *node);

private:
  mutable std::shared_mutex mutex_;
  std::vector<Node *> nodes_;
};

} // namespace nanobdd

// Inline implementations
namespace nanobdd {

inline Node *Bucket::find(uint32_t level, Node *low, Node *high) const {
  std::shared_lock<std::shared_mutex> lock(mutex_);
  for (auto *node : nodes_) {
    if (node->low == low && node->high == high && node->level == level) {
      return node;
    }
  }
  return nullptr;
}

inline std::pair<Node *, bool>
Bucket::insertIfAbsent(uint32_t level, Node *low, Node *high, Node *candidate) {
  std::unique_lock<std::shared_mutex> lock(mutex_);
  for (auto *node : nodes_) {
    if (node->low == low && node->high == high && node->level == level) {
      return {node, false};
    }
  }
  nodes_.push_back(candidate);
  return {candidate, true};
}

inline size_t Bucket::numNodes() const {
  std::shared_lock<std::shared_mutex> lock(mutex_);
  return nodes_.size();
}

inline void Bucket::markNodesUnsafe() {
  for (auto *node : nodes_) {
    if (*(node->refCount) != 0) {
      node->markRec();
    }
  }
}

inline size_t Bucket::sweepNodesUnsafe(std::vector<Node *> &freed) {
  size_t removed = 0;
  auto it = nodes_.begin();
  while (it != nodes_.end()) {
    Node *node = *it;
    if (!node->inUse && node->refCount->load(std::memory_order_relaxed) == 0) {
      freed.push_back(node);
      it = nodes_.erase(it);
      ++removed;
    } else {
      ++it;
    }
  }
  return removed;
}

inline void Bucket::unmarkNodesUnsafe() {
  for (auto *node : nodes_) {
    node->inUse = false;
  }
}

inline void Bucket::debugNodes() const {
  std::shared_lock<std::shared_mutex> lock(mutex_);
  for (auto *node : nodes_) {
    std::cout << "Node: " << node->level << " " << node << " " << node->low
              << " " << node->high << " " << node->refCount->load() << " "
              << node->inUse << std::endl;
  }
}

inline void Bucket::extractAllNodes(std::vector<Node *> &out) {
  std::unique_lock<std::shared_mutex> lock(mutex_);
  out.insert(out.end(), nodes_.begin(), nodes_.end());
  nodes_.clear();
}

inline void Bucket::insertExisting(Node *node) { nodes_.push_back(node); }

} // namespace nanobdd
