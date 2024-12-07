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
    for (auto p = listHead_.load(); p != nullptr; p = p->next) {
      std::cout << "Node: " << p->bddNode.level << " " <<
      p->bddNode.low << " " << p->bddNode.high << " " <<
      p->bddNode.refCount->load() << " " << p->bddNode.inUse << std::endl;
    }
  }

  void markNodes() {
    for (auto p = listHead_.load(); p != nullptr; p = p->next) {
        p->bddNode.mark();
    }
  }

  void unmarkNodes() {
    for (auto p = listHead_.load(); p != nullptr; p = p->next) {
        p->bddNode.unmark();
    }
  }

  void freeNodes() {
    // free nodes that are not in use
    ListNode* head = nullptr;
    ListNode* p = listHead_.load();
    while (p != nullptr) {
      if (p->bddNode.inUse == false) {
        ListNode* next = p->next;
        delete p;
        p = next;
      } else {
        if (head == nullptr) {
          head = p;
        }
        p = p->next;
      }
    }
    listHead_.store(head);
  }

 private:
  std::atomic<ListNode*> listHead_{nullptr};
};

} // namespace nanobdd