#pragma once

#include <nanobdd/Node.h>
#include <atomic>
#include <iostream>

#define NANOBDD_LOCK_FREE

namespace nanobdd {

struct ListNode {
  Node bddNode;
  ListNode* next;
};

    class LockFreeBucket {
 public:
  LockFreeBucket() {}

  // lock-free approach
  Node*
  operator()(int level, Node* low, Node* high) {
    ListNode *oldHead = listHead_.load();
    for (auto p = oldHead; p != nullptr; p = p->next) {
      if (p->bddNode.low == low && p->bddNode.high == high && p->bddNode.level == level) {
        return &p->bddNode;
      }
    }

    // auto node = new Node(level, low, high);
    auto newListNode = new ListNode();
    newListNode->bddNode.level = level;
    newListNode->bddNode.low = low;
    newListNode->bddNode.high = high;
    newListNode->next = oldHead;

    // some other threads insert new nodes
    while (listHead_.compare_exchange_weak(newListNode->next, newListNode) == false) {
      // auto head = listHead_.load();
      for (auto p = newListNode->next; p != oldHead; p = p->next) {
        if (p->bddNode.low == low && p->bddNode.high == high && p->bddNode.level == level) {
          free(newListNode);
          return &p->bddNode;
        }
      }
      oldHead = newListNode->next;
    } 

    return &newListNode->bddNode;
  }

  size_t numNodes() {
    size_t cnt = 0;
    for (auto p = listHead_.load(); p != nullptr; p = p->next) {
      cnt++;
    }
    return cnt;
  }

  void debugNodes() {
    ListNode* p = listHead_.load();
    while (p != nullptr) {
      std::cout << "Node: " << p->bddNode.level << " " <<
      &(p->bddNode) << " " <<
      p->bddNode.low << " " << p->bddNode.high << " " <<
      p->bddNode.refCount.load(std::memory_order_seq_cst) << " "
      << p->bddNode.inUse << std::endl;
      p = p->next;
    }
  }

  void markNodes() {
    ListNode* p = listHead_.load();
    while (p != nullptr) {
      if (p->bddNode.refCount.load(std::memory_order_seq_cst) != 0) {
        p->bddNode.markRec();
      }
      p = p->next;
    }
  }

  void unmarkNodes() {
    ListNode* p = listHead_.load();
    while (p != nullptr) {
      p->bddNode.unmark();
      p = p->next;
    }
  }

  // free nodes that are not in use
  void freeNodes() {
    ListNode* p = listHead_.load();

    // if the beginning nodes are not in use, delete them
    while (p != nullptr && !p->bddNode.inUse) {
      ListNode* tmp = p->next;
      delete p;
      p = tmp;
    }

    // then p should either be 1. nullptr or 2. not null and in use
    listHead_.store(p);

    // if p is nullptr, early return
    if (p == nullptr) {
      return;
    }

    // if p is not nullptr, we should delete following nodes that are not in use
    ListNode* tmp;
    while (p->next != nullptr) {
      if (!p->next->bddNode.inUse) {
        tmp = p->next->next;
        delete p->next;
        p->next = tmp;
      } else {
        p = p->next;
      }
    }
  }

 private:
  std::atomic<ListNode*> listHead_{nullptr};
};

} // namespace nanobdd
