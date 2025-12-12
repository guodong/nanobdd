#include <Cache.h>
#include <Common.h>
#include <Hash.h>
#include <LockFreeCache.h>
#include <NodeTable.h>
#include <Prime.h>

#include <algorithm>
#include <climits>
#include <shared_mutex>

namespace nanobdd {

#ifdef NANOBDD_LOCK_FREE_CACHE
extern LockFreeCache *cache;
#else
extern Cache *cache;
#endif

NodeTable::NodeTable(size_t tableSize)
    : tableSize_(tableSize), allocator_(tableSize) {
  buckets_.reserve(tableSize_);
  for (size_t i = 0; i < tableSize_; ++i) {
    buckets_.push_back(std::make_unique<Bucket>());
  }
  falseNode_ = getOrCreateNode(UINT_MAX, nullptr, nullptr);
  trueNode_ = getOrCreateNode(UINT_MAX - 1, nullptr, nullptr);

  // terminal nodes always exist
  falseNode_->ref();
  trueNode_->ref();
}

size_t NodeTable::hash(uint32_t level, Node *low, Node *high) const {
  return HASH_O_3(level, reinterpret_cast<uintptr_t>(low),
                  reinterpret_cast<uintptr_t>(high)) %
         tableSize_;
}

Node *NodeTable::createVar(uint32_t id) {
  auto node = getOrCreateNode(id, falseNode_, trueNode_);
  // variables always exist
  node->ref();
  vars_.emplace(vars_.begin() + id, node);
  auto nvar = getOrCreateNode(id, trueNode_, falseNode_);
  // negated variables always exist
  nvar->ref();
  nvars_.emplace(nvars_.begin() + id, nvar);
  return vars_.at(id);
}

inline bool NodeTable::isFalse(const Node *node) const {
  return node == falseNode_;
}

inline bool NodeTable::isTrue(const Node *node) const {
  return node == trueNode_;
}

Node *NodeTable::bddAnd(Node *x, Node *y) {
  if (x == y) {
    return x;
  }
  if (isFalse(x) || isFalse(y)) {
    return falseNode_;
  }
  if (isTrue(x)) {
    return y;
  }
  if (isTrue(y)) {
    return x;
  }
  uint32_t m = std::min(x->level, y->level);

  auto hashVal = HASH_UO_O_3(reinterpret_cast<uintptr_t>(x),
                             reinterpret_cast<uintptr_t>(y), Operator::AND) %
                 cache->size();
  auto cached = cache->lookup(hashVal, x, y, Operator::AND);
  if (cached) {
    return cached;
  }

  auto fLow = bddAnd(negCof(x, m), negCof(y, m));
  auto fHigh = bddAnd(posCof(x, m), posCof(y, m));

  auto res = combine(m, fLow, fHigh);

  cache->insert(hashVal, res, x, y, Operator::AND);

  return res;
}

Node *NodeTable::bddOr(Node *x, Node *y) {
  if (x == y) {
    return x;
  }
  if (isTrue(x) || isTrue(y)) {
    return trueNode_;
  }
  if (isFalse(x)) {
    return y;
  }
  if (isFalse(y)) {
    return x;
  }
  int m = std::min(x->level, y->level);

  auto hashVal = HASH_UO_O_3(reinterpret_cast<uintptr_t>(x),
                             reinterpret_cast<uintptr_t>(y), Operator::OR) %
                 cache->size();
  auto cached = cache->lookup(hashVal, x, y, Operator::OR);
  if (cached) {
    return cached;
  }

  auto fLow = bddOr(negCof(x, m), negCof(y, m));
  auto fHigh = bddOr(posCof(x, m), posCof(y, m));

  auto res = combine(m, fLow, fHigh);

  cache->insert(hashVal, res, x, y, Operator::OR);

  return res;
}

Node *NodeTable::bddNot(Node *x) {
  if (isTrue(x)) {
    return falseNode_;
  }
  if (isFalse(x)) {
    return trueNode_;
  }

  auto hashVal = HASH_UO_O_3(reinterpret_cast<uintptr_t>(x), 0, Operator::NOT) %
                 cache->size();
  auto cached = cache->lookup(hashVal, x, nullptr, Operator::NOT);
  if (cached) {
    return cached;
  }

  int m = x->level;

  auto fLow = bddNot(negCof(x, m));
  auto fHigh = bddNot(posCof(x, m));

  auto res = combine(m, fLow, fHigh);

  cache->insert(hashVal, res, x, nullptr, Operator::NOT);

  return res;
}

Node *NodeTable::bddXor(Node *x, Node *y) {
  if (x == y) {
    return falseNode_;
  }
  if (isFalse(x)) {
    return y;
  }
  if (isFalse(y)) {
    return x;
  }
  int m = std::min(x->level, y->level);

  auto hashVal = HASH_UO_O_3(reinterpret_cast<uintptr_t>(x),
                             reinterpret_cast<uintptr_t>(y), Operator::XOR) %
                 cache->size();
  auto cached = cache->lookup(hashVal, x, y, Operator::XOR);
  if (cached) {
    return cached;
  }

  auto fLow = bddXor(negCof(x, m), negCof(y, m));
  auto fHigh = bddXor(posCof(x, m), posCof(y, m));

  auto res = combine(m, fLow, fHigh);

  cache->insert(hashVal, res, x, y, Operator::XOR);

  return res;
}

Node *NodeTable::bddDiff(Node *x, Node *y) {
  if (x == y || isFalse(x) || isTrue(y)) {
    return falseNode_;
  }
  if (isFalse(y)) {
    return x;
  }

  int m = std::min(x->level, y->level);

  auto hashVal = HASH_O_3(reinterpret_cast<uintptr_t>(x),
                          reinterpret_cast<uintptr_t>(y), Operator::DIFF) %
                 cache->size();
  auto cached = cache->lookup(hashVal, x, y, Operator::DIFF);
  if (cached) {
    return cached;
  }

  auto fLow = bddDiff(negCof(x, m), negCof(y, m));
  auto fHigh = bddDiff(posCof(x, m), posCof(y, m));

  auto res = combine(m, fLow, fHigh);

  cache->insert(hashVal, res, x, y, Operator::DIFF);

  return res;
}

Node *NodeTable::bddImp(Node *x, Node *y) {
  if (isFalse(x) || isTrue(y)) {
    return trueNode_;
  }
  if (isTrue(x)) {
    return y;
  }

  int m = std::min(x->level, y->level);

  auto hashVal = HASH_O_3(reinterpret_cast<uintptr_t>(x),
                          reinterpret_cast<uintptr_t>(y), Operator::IMP) %
                 cache->size();
  auto cached = cache->lookup(hashVal, x, y, Operator::IMP);
  if (cached) {
    return cached;
  }

  auto fLow = bddDiff(negCof(x, m), negCof(y, m));
  auto fHigh = bddDiff(posCof(x, m), posCof(y, m));

  auto res = combine(m, fLow, fHigh);

  cache->insert(hashVal, res, x, y, Operator::IMP);

  return res;
}

inline Node *NodeTable::negCof(Node *x, int id) {
  if (id < x->level) {
    return x;
  }
  return x->low;
}

inline Node *NodeTable::posCof(Node *x, int id) {
  if (id < x->level) {
    return x;
  }
  return x->high;
}

inline Node *NodeTable::combine(uint32_t level, Node *low, Node *high) {
  if (low == high) {
    return low;
  }
  return getOrCreateNode(level, low, high);
}

Node *NodeTable::getOrCreateNode(uint32_t level, Node *low, Node *high) {
  while (true) {
    std::shared_lock<std::shared_mutex> tableLock(tableMutex_);

    auto idx = hash(level, low, high);
    auto &bucket = buckets_.at(idx);

    if (auto *existing = bucket->find(level, low, high)) {
      return existing;
    }

    Node *candidate = allocator_.allocate(level, low, high);
    if (candidate == nullptr) {
      tableLock.unlock();
      performMaintenance();
      continue;
    }

    auto [node, inserted] = bucket->insertIfAbsent(level, low, high, candidate);
    if (!inserted) {
      allocator_.free(candidate);
      return node;
    }

    nodeCount_.fetch_add(1, std::memory_order_relaxed);

    tableLock.unlock();
    maybeGrowBuckets();
    return node;
  }
}

void NodeTable::performMaintenance() {
  std::unique_lock<std::shared_mutex> lock(tableMutex_);

  size_t freed = gcLocked();
  if (!allocator_.hasFreeSlot()) {
    allocator_.grow();
  }

  if (shouldGrowBucketsLocked()) {
    auto newSize = bdd_prime_gte(static_cast<unsigned int>(tableSize_ * 2 + 1));
    growBucketsLocked(newSize);
  }
}

size_t NodeTable::gcLocked() {
  std::vector<Node *> freed;
  freed.reserve(nodeCount_.load(std::memory_order_relaxed));

  for (auto &bucket : buckets_) {
    bucket->markNodesUnsafe();
  }

  size_t removed = 0;
  for (auto &bucket : buckets_) {
    removed += bucket->sweepNodesUnsafe(freed);
  }

  for (auto &bucket : buckets_) {
    bucket->unmarkNodesUnsafe();
  }

  allocator_.free(freed);
  nodeCount_.fetch_sub(removed, std::memory_order_relaxed);

#ifdef NANOBDD_LOCK_FREE_CACHE
  cache->invalidateAll();
#endif
  return removed;
}

bool NodeTable::shouldGrowBucketsLocked() const {
  if (tableSize_ == 0) {
    return false;
  }
  double load =
      static_cast<double>(nodeCount_.load(std::memory_order_relaxed)) /
      static_cast<double>(tableSize_);
  return load > maxLoadFactor_;
}

void NodeTable::growBucketsLocked(size_t newSize) {
  if (newSize <= tableSize_) {
    return;
  }

  std::vector<Node *> nodes;
  nodes.reserve(nodeCount_.load(std::memory_order_relaxed));
  for (auto &bucket : buckets_) {
    bucket->extractAllNodes(nodes);
  }

  buckets_.clear();
  buckets_.reserve(newSize);
  for (size_t i = 0; i < newSize; ++i) {
    buckets_.push_back(std::make_unique<Bucket>());
  }
  tableSize_ = newSize;

  for (auto *node : nodes) {
    auto idx = hash(node->level, node->low, node->high);
    buckets_.at(idx)->insertExisting(node);
  }
}

void NodeTable::maybeGrowBuckets() {
  std::unique_lock<std::shared_mutex> lock(tableMutex_, std::try_to_lock);
  if (!lock.owns_lock()) {
    return;
  }

  if (!shouldGrowBucketsLocked()) {
    return;
  }

  auto newSize = bdd_prime_gte(static_cast<unsigned int>(tableSize_ * 2 + 1));
  growBucketsLocked(newSize);
}

Node *NodeTable::operator()(uint32_t level, Node *low, Node *high) {
  return getOrCreateNode(level, low, high);
}

void NodeTable::gc() {
  std::unique_lock<std::shared_mutex> lock(tableMutex_);
  gcLocked();
}

void NodeTable::debugNodes() {
  for (auto &bucket : buckets_) {
    bucket->debugNodes();
  }
}

} // namespace nanobdd
