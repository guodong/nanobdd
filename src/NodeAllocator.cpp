#include <NodeAllocator.h>

#include <algorithm>

namespace nanobdd {
NodeAllocator::Slab::Slab(size_t cap)
    : nodes(std::make_unique<Node[]>(cap)), capacity(cap), next(0) {}

NodeAllocator::NodeAllocator(size_t initialCapacity, size_t growthFactor)
    : nextSlabCapacity_(std::max<size_t>(1, initialCapacity)),
      growthFactor_(std::max<size_t>(2, growthFactor)) {
  auto slab = std::make_unique<Slab>(nextSlabCapacity_);
  currentSlab_.store(slab.get(), std::memory_order_relaxed);
  slabs_.push_back(std::move(slab));
  nextSlabCapacity_ = std::max<size_t>(nextSlabCapacity_ * growthFactor_, 1);
}

Node *NodeAllocator::allocate(uint32_t level, Node *low, Node *high) {
  if (auto *reused = popFree()) {
    resetNode(reused, level, low, high);
    return reused;
  }

  auto *slab = currentSlab_.load(std::memory_order_acquire);
  if (auto *fresh = allocateFromSlab(slab, level, low, high)) {
    return fresh;
  }

  // Caller will decide whether to trigger GC or grow.
  return nullptr;
}

void NodeAllocator::free(Node *node) {
  if (node == nullptr) {
    return;
  }
  node->low = nullptr;
  node->high = nullptr;
  node->inUse = false;
  node->refCount.store(0, std::memory_order_relaxed);
  pushFree(node);
}

void NodeAllocator::free(const std::vector<Node *> &nodes) {
  for (auto *node : nodes) {
    free(node);
  }
}

bool NodeAllocator::hasFreeSlot() const {
  if (freeList_.load(std::memory_order_acquire) != nullptr) {
    return true;
  }
  auto *slab = currentSlab_.load(std::memory_order_acquire);
  if (slab == nullptr) {
    return false;
  }
  return slab->next.load(std::memory_order_relaxed) < slab->capacity;
}

size_t NodeAllocator::capacity() const {
  size_t sum = 0;
  for (const auto &slab : slabs_) {
    sum += slab->capacity;
  }
  return sum;
}

void NodeAllocator::grow() {
  std::lock_guard<std::mutex> lock(slabMutex_);
  auto slab = std::make_unique<Slab>(nextSlabCapacity_);
  auto *slabPtr = slab.get();
  slabs_.push_back(std::move(slab));
  currentSlab_.store(slabPtr, std::memory_order_release);
  nextSlabCapacity_ = std::max<size_t>(nextSlabCapacity_ * growthFactor_, 1);
}

Node *NodeAllocator::allocateFromSlab(Slab *slab, uint32_t level, Node *low,
                                      Node *high) {
  if (slab == nullptr) {
    return nullptr;
  }

  size_t idx = slab->next.load(std::memory_order_relaxed);
  while (idx < slab->capacity) {
    if (slab->next.compare_exchange_weak(idx, idx + 1,
                                         std::memory_order_acq_rel,
                                         std::memory_order_relaxed)) {
      auto *node = &slab->nodes[idx];
      resetNode(node, level, low, high);
      return node;
    }
    // idx updated by compare_exchange_weak when it fails; loop continues.
  }
  return nullptr;
}

Node *NodeAllocator::popFree() {
  Node *head = freeList_.load(std::memory_order_acquire);
  while (head != nullptr) {
    Node *next = head->nextFree;
    if (freeList_.compare_exchange_weak(head, next, std::memory_order_acq_rel,
                                        std::memory_order_acquire)) {
      head->nextFree = nullptr;
      return head;
    }
  }
  return nullptr;
}

void NodeAllocator::pushFree(Node *node) {
  Node *head = freeList_.load(std::memory_order_relaxed);
  do {
    node->nextFree = head;
  } while (!freeList_.compare_exchange_weak(
      head, node, std::memory_order_release, std::memory_order_relaxed));
}

void NodeAllocator::resetNode(Node *node, uint32_t level, Node *low,
                              Node *high) {
  node->level = level;
  node->low = low;
  node->high = high;
  node->inUse = false;
  node->nextFree = nullptr;
  node->refCount.store(0, std::memory_order_relaxed);
}
} // namespace nanobdd
