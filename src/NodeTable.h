#pragma once

#include <Bucket.h>
#include <Common.h>
#include <LockFreeBucket.h>
#include <nanobdd/Node.h>
#include <atomic>

namespace nanobdd {

class NodeTable {
 public:
  NodeTable(size_t tableSize);

  Node*
  falseNode() {
    return falseNode_;
  }

  Node*
  trueNode() {
    return trueNode_;
  }

  Node* createVar(uint32_t id);

  Node*
  getVar(uint32_t id) {
    return vars_.at(id);
  };
  
  Node*
  getNvar(uint32_t id) {
    return nvars_.at(id);
  };

  Node* bddAnd(Node* x, Node* y);
  Node* bddOr(Node* x, Node* y);
  Node* bddNot(Node* x);
  Node* bddXor(Node* x, Node* y);
  Node* bddDiff(Node* x, Node* y);
  Node* bddImp(Node* x, Node* y);

  /**
   * Get or create a node
   */
  Node* operator()(uint32_t level, Node* low, Node* high);

  size_t
  numNodes() {
    size_t cnt = 0;
    for (auto& bucket : buckets_) {
      cnt += bucket.numNodes();
    }
    return cnt;
  }

  void gc();

  void debugNodes();

 private:
  Node* getOrCreateNode(uint32_t level, Node* low, Node* high);
  Node* combine(uint32_t level, Node* low, Node* high);
  bool isFalse(const Node* node) const;
  bool isTrue(const Node* node) const;

  Node* negCof(Node* x, int id);

  Node* posCof(Node* x, int id);

  size_t tableSize_;

#ifdef NANOBDD_LOCK_FREE_BUCKET
  std::vector<LockFreeBucket> buckets_;
#else
  std::vector<Bucket> buckets_;
#endif
  Node* falseNode_;
  Node* trueNode_;
  std::vector<Node*> vars_;
  std::vector<Node*> nvars_;
};

} // namespace nanobdd