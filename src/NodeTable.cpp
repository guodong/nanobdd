#include <Cache.h>
#include <Common.h>
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

inline bool
NodeTable::isFalse(const Node* node) const {
  return node == falseNode_;
}

inline bool
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
          reinterpret_cast<uintptr_t>(x), reinterpret_cast<uintptr_t>(y), Operator::AND) %
      cache->size();
  auto cached = cache->lookup(hash, x, y, Operator::AND);
  if (cached) {
    return cached;
  }

  auto fLow = bddAnd(negCof(x, m), negCof(y, m));
  auto fHigh = bddAnd(posCof(x, m), posCof(y, m));

  auto res = combine(m, fLow, fHigh);

  cache->insert(hash, res, x, y, Operator::AND);

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
          reinterpret_cast<uintptr_t>(x), reinterpret_cast<uintptr_t>(y), Operator::OR) %
      cache->size();
  auto cached = cache->lookup(hash, x, y, Operator::OR);
  if (cached) {
    return cached;
  }

  auto fLow = bddOr(negCof(x, m), negCof(y, m));
  auto fHigh = bddOr(posCof(x, m), posCof(y, m));

  auto res = combine(m, fLow, fHigh);

  cache->insert(hash, res, x, y, Operator::OR);

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
  auto hash = HASH_UO_O_3(reinterpret_cast<uintptr_t>(x), 0, Operator::NOT) % cache->size();
  auto cached = cache->lookup(hash, x, nullptr, Operator::NOT);
  if (cached) {
    return cached;
  }

  int m = x->level;

  auto fLow = bddNot(negCof(x, m));
  auto fHigh = bddNot(posCof(x, m));

  auto res = combine(m, fLow, fHigh);

  cache->insert(hash, res, x, nullptr, Operator::NOT);

  return res;
}

Node*
NodeTable::bddXor(Node* x, Node* y) {
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

  auto hash =
      HASH_UO_O_3(
          reinterpret_cast<uintptr_t>(x), reinterpret_cast<uintptr_t>(y), Operator::XOR) %
      cache->size();
  auto cached = cache->lookup(hash, x, y, Operator::XOR);
  if (cached) {
    return cached;
  }

  auto fLow = bddXor(negCof(x, m), negCof(y, m));
  auto fHigh = bddXor(posCof(x, m), posCof(y, m));

  auto res = combine(m, fLow, fHigh);

  cache->insert(hash, res, x, y, Operator::XOR);

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
          reinterpret_cast<uintptr_t>(x), reinterpret_cast<uintptr_t>(y), Operator::DIFF) %
      cache->size();
  auto cached = cache->lookup(hash, x, y, Operator::DIFF);
  if (cached) {
    return cached;
  }

  auto fLow = bddDiff(negCof(x, m), negCof(y, m));
  auto fHigh = bddDiff(posCof(x, m), posCof(y, m));

  auto res = combine(m, fLow, fHigh);

  cache->insert(hash, res, x, y, Operator::DIFF);

  return res;
}

Node*
NodeTable::bddImp(Node* x, Node* y) {
  if (isFalse(x) || isTrue(y)) {
    return trueNode_;
  }
  if (isTrue(x)) {
    return y;
  }

  int m = std::min(x->level, y->level);

  auto hash =
      HASH_UO_O_3(
          reinterpret_cast<uintptr_t>(x), reinterpret_cast<uintptr_t>(y), Operator::IMP) %
      cache->size();
  auto cached = cache->lookup(hash, x, y, Operator::IMP);
  if (cached) {
    return cached;
  }

  auto fLow = bddDiff(negCof(x, m), negCof(y, m));
  auto fHigh = bddDiff(posCof(x, m), posCof(y, m));

  auto res = combine(m, fLow, fHigh);

  cache->insert(hash, res, x, y, Operator::IMP);

  return res;
}

inline Node*
NodeTable::negCof(Node* x, int id) {
  // check id <= root_
  if (id < x->level) {
    return x;
  } else { // id == root_
    return x->low;
  }
}

inline Node*
NodeTable::posCof(Node* x, int id) {
  // check id <= root_
  if (id < x->level) {
    return x;
  } else { // id == root_
    return x->high;
  }
}

inline Node*
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