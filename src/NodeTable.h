#pragma once

#include <nanobdd/Node.h>
#include <tbb/concurrent_vector.h>
#include <atomic>
#include <shared_mutex>

#define NANOBDD_LOCK_FREE

namespace nanobdd {

struct ListNode {
  Node bddNode;
  ListNode* next;
};

class Bucket {
 public:
  Bucket() : mutex_(std::make_unique<std::mutex>()) {}

#ifndef NANOBDD_LOCK_FREE
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
#endif

#ifdef NANOBDD_LOCK_FREE
  // lock-free approach
  Node*
  operator()(int level, Node* low, Node* high) {
    for (auto p = listHead_.load(); p != nullptr; p = p->next) {
      if (p->bddNode.low == low && p->bddNode.high == high && p->bddNode.level == level) {
        return &p->bddNode;
      }
    }

    // auto node = new Node(level, low, high);
    ListNode *oldHead = listHead_.load();
    auto newListNode = new ListNode();
    newListNode->bddNode.level = level;
    newListNode->bddNode.low = low;
    newListNode->bddNode.high = high;
    newListNode->next = oldHead;

    // some other threads insert new nodes
    while (listHead_.compare_exchange_weak(oldHead, newListNode) == false) {
      // auto head = listHead_.load();
      for (auto p = oldHead; p != newListNode->next && p != nullptr; p = p->next) {
        if (p->bddNode.low == low && p->bddNode.high == high && p->bddNode.level == level) {
          free(newListNode);
          return &p->bddNode;
        }
      }
      
      newListNode->next = oldHead;
    } 

    return &newListNode->bddNode;
  }

 private:
  tbb::concurrent_vector<Node> nodes_;
  std::unique_ptr<std::mutex> mutex_;

  std::atomic<ListNode*> listHead_;
};
#endif

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
  Node* getVar(uint32_t id) {
    return vars_.at(id);
  };
  Node* getNvar(uint32_t id) {
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

 private:
  Node* getOrCreateNode(uint32_t level, Node* low, Node* high);
  Node* combine(uint32_t level, Node* low, Node* high);
  bool isFalse(const Node* node) const;
  bool isTrue(const Node* node) const;

  Node* negCof(Node* x, int id);

  Node* posCof(Node* x, int id);

  size_t tableSize_;
  std::vector<Bucket> buckets_;
  Node* falseNode_;
  Node* trueNode_;
  std::vector<Node*> vars_;
  std::vector<Node*> nvars_;
};

} // namespace nanobdd