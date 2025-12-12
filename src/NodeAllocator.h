#pragma once

#include <nanobdd/Node.h>

#include <atomic>
#include <memory>
#include <mutex>
#include <vector>

namespace nanobdd {
// A simple slab-based allocator for BDD nodes. It hands out stable Node*
// addresses and recycles freed nodes via a lock-free free list. Slab growth is
// driven by the caller (NodeTable) so we can attempt GC before allocating more
// memory.
class NodeAllocator {
public:
  explicit NodeAllocator(size_t initialCapacity, size_t growthFactor = 2);

  // Attempts to allocate a node; returns nullptr if the current capacity is
  // exhausted (free list empty and active slab full).
  Node *allocate(uint32_t level, Node *low, Node *high);

  // Push a single node or a batch of nodes back to the free list.
  void free(Node *node);

  void free(const std::vector<Node *> &nodes);

  // Adds a new slab, increasing capacity. Caller controls when growth
  // happens.
  void grow();

  // Checks whether allocation could succeed without growing.
  bool hasFreeSlot() const;

  size_t capacity() const;

private:
  struct Slab {
    explicit Slab(size_t capacity);

    std::unique_ptr<Node[]> nodes;
    const size_t capacity;
    std::atomic<size_t> next;
  };

  Node *allocateFromSlab(Slab *slab, uint32_t level, Node *low, Node *high);

  Node *popFree();

  void pushFree(Node *node);

  static void resetNode(Node *node, uint32_t level, Node *low, Node *high);

  std::vector<std::unique_ptr<Slab>> slabs_;
  mutable std::mutex slabMutex_;
  std::atomic<Slab *> currentSlab_{nullptr};
  std::atomic<Node *> freeList_{nullptr};
  size_t nextSlabCapacity_;
  const size_t growthFactor_;
};
} // namespace nanobdd
