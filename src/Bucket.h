#pragma once

#include <nanobdd/Node.h>
#include <tbb/concurrent_vector.h>
#include <shared_mutex>

namespace nanobdd {

class Bucket {
 public:
  Bucket() : mutex_(std::make_unique<std::mutex>()) {}

  // TBB approach
  Node*
  operator()(int level, Node* low, Node* high) {
    for (auto& node : nodes_) {
      if (node.low == low && node.high == high && node.level == level) {
        return &node;
      }
    }
    std::lock_guard<std::mutex> lock(*mutex_);
    for (auto& node : nodes_) {
      if (node.low == low && node.high == high && node.level == level) {
        return &node;
      }
    }
    Node node;
    node.level = level;
    node.low = low;
    node.high = high;
    auto it = nodes_.push_back(std::move(node));
    return &(*it);
  }

  size_t numNodes() {
    return nodes_.size();
  }

 private:
  tbb::concurrent_vector<Node> nodes_;
  std::unique_ptr<std::mutex> mutex_;
};

} // namespace nanobdd