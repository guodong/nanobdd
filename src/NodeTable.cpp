#include <Cache.h>
#include <Hash.h>
#include <NodeTable.h>

namespace nanobdd {

extern Cache* cache;

NodeTable::NodeTable(size_t tableSize) : tableSize_(tableSize), buckets_(tableSize) {
  falseNode_ = getOrCreateNode(UINT_MAX, nullptr, nullptr);
  trueNode_ = getOrCreateNode(UINT_MAX - 1, nullptr, nullptr);
}

Node*
NodeTable::createVar(uint32_t id) {
  auto node = getOrCreateNode(id, falseNode_, trueNode_);
  vars_.emplace(vars_.begin() + id, node);
  auto nvar = getOrCreateNode(id, trueNode_, falseNode_);
  nvars_.emplace(nvars_.begin() + id, nvar);
  return vars_.at(id);
}

bool
NodeTable::isFalse(const Node* node) const {
  return node == falseNode_;
}

bool
NodeTable::isTrue(const Node* node) const {
  return node == trueNode_;
}

Node*
NodeTable::bddAnd(Node* x, Node* y) {
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

  auto hash =
      HASH_UO_O_3(
          reinterpret_cast<uintptr_t>(x), reinterpret_cast<uintptr_t>(y), 0) %
      cache->size();
  auto cached = cache->lookup(hash, x, y, 0);
  if (cached) {
    return cached;
  }

  auto fLow = bddAnd(negCof(x, m), negCof(y, m));
  auto fHigh = bddAnd(posCof(x, m), posCof(y, m));

  auto res = combine(m, fLow, fHigh);

  cache->insert(hash, res, x, y, 0);

  return res;
}

Node*
NodeTable::bddOr(Node* x, Node* y) {
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

  auto hash =
      HASH_UO_O_3(
          reinterpret_cast<uintptr_t>(x), reinterpret_cast<uintptr_t>(y), 1) %
      cache->size();
  auto cached = cache->lookup(hash, x, y, 1);
  if (cached) {
    return cached;
  }

  auto fLow = bddOr(negCof(x, m), negCof(y, m));
  auto fHigh = bddOr(posCof(x, m), posCof(y, m));

  auto res = combine(m, fLow, fHigh);

  cache->insert(hash, res, x, y, 1);

  return res;
}

Node*
NodeTable::bddNot(Node* x) {
  if (isTrue(x)) {
    return falseNode_;
  }
  if (isFalse(x)) {
    return trueNode_;
  }

  // auto hash = TRIPLEp(x.node(), nullptr, 2) % Nanobdd::cacheSize_;
  auto hash = HASH_UO_O_3(reinterpret_cast<uintptr_t>(x), 0, 2) % cache->size();
  auto cached = cache->lookup(hash, x, nullptr, 2);
  if (cached) {
    return cached;
  }

  int m = x->level;

  auto fLow = bddNot(negCof(x, m));
  auto fHigh = bddNot(posCof(x, m));

  auto res = combine(m, fLow, fHigh);

  cache->insert(hash, res, x, nullptr, 2);

  return res;
}

Node*
NodeTable::bddDiff(Node* x, Node* y) {
  if (x == y || isFalse(x) || isTrue(y)) {
    return falseNode_;
  }
  if (isFalse(y)) {
    return x;
  }

  int m = std::min(x->level, y->level);

  auto hash =
      HASH_UO_O_3(
          reinterpret_cast<uintptr_t>(x), reinterpret_cast<uintptr_t>(y), 3) %
      cache->size();
  auto cached = cache->lookup(hash, x, y, 3);
  if (cached) {
    return cached;
  }

  auto fLow = bddDiff(negCof(x, m), negCof(y, m));
  auto fHigh = bddDiff(posCof(x, m), posCof(y, m));

  auto res = combine(m, fLow, fHigh);

  cache->insert(hash, res, x, y, 3);

  return res;
}

Node*
NodeTable::negCof(Node* x, int id) {
  // check id <= root_
  if (id < x->level) {
    return x;
  } else { // id == root_
    return x->low;
  }
}

Node*
NodeTable::posCof(Node* x, int id) {
  // check id <= root_
  if (id < x->level) {
    return x;
  } else { // id == root_
    return x->high;
  }
}

Node*
NodeTable::combine(uint32_t level, Node* low, Node* high) {
  if (low == high) {
    return low;
  }
  return getOrCreateNode(level, low, high);
}

Node*
NodeTable::getOrCreateNode(uint32_t level, Node* low, Node* high) {
  auto hash = HASH_O_3(
                  level,
                  reinterpret_cast<uintptr_t>(low),
                  reinterpret_cast<uintptr_t>(high)) %
      tableSize_;
  auto& bucket = buckets_.at(hash);

  return bucket(level, low, high);
}

Node*
NodeTable::operator()(uint32_t level, Node* low, Node* high) {
  return getOrCreateNode(level, low, high);
}

} // namespace nanobdd