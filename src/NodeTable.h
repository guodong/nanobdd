#pragma once

#include <Bucket.h>
#include <Common.h>
#include <NodeAllocator.h>
#include <atomic>
#include <memory>
#include <nanobdd/Node.h>
#include <shared_mutex>
#include <vector>

namespace nanobdd {

class NodeTable {
public:
  NodeTable(size_t tableSize);

  Node *falseNode() { return falseNode_; }

  Node *trueNode() { return trueNode_; }

  Node *createVar(uint32_t id);

  Node *getVar(uint32_t id) { return vars_.at(id); };

  Node *getNvar(uint32_t id) { return nvars_.at(id); };

  Node *bddAnd(Node *x, Node *y);
  Node *bddOr(Node *x, Node *y);
  Node *bddNot(Node *x);
  Node *bddXor(Node *x, Node *y);
  Node *bddDiff(Node *x, Node *y);
  Node *bddImp(Node *x, Node *y);

  /**
   * Get or create a node
   */
  Node *operator()(uint32_t level, Node *low, Node *high);

  size_t numNodes() { return nodeCount_.load(std::memory_order_relaxed); }

  void gc();

  void debugNodes();

private:
  Node *getOrCreateNode(uint32_t level, Node *low, Node *high);
  Node *combine(uint32_t level, Node *low, Node *high);
  bool isFalse(const Node *node) const;
  bool isTrue(const Node *node) const;

  size_t hash(uint32_t level, Node *low, Node *high) const;
  void performMaintenance();
  size_t gcLocked();
  bool shouldGrowBucketsLocked() const;
  void growBucketsLocked(size_t newSize);
  void maybeGrowBuckets();

  Node *negCof(Node *x, uint32_t id);

  Node *posCof(Node *x, uint32_t id);

  size_t tableSize_;
  std::vector<std::unique_ptr<Bucket>> buckets_;
  std::shared_mutex tableMutex_;
  NodeAllocator allocator_;
  std::atomic<size_t> nodeCount_{0};
  double maxLoadFactor_{4.0};
  Node *falseNode_;
  Node *trueNode_;
  std::vector<Node *> vars_;
  std::vector<Node *> nvars_;
};

} // namespace nanobdd
