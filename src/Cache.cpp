#include <Cache.h>

namespace nanobdd {

Node*
Cache::lookup(uint32_t hash, Node* left, Node* right, Operator op) {
  auto& cache = getCache(op);
  auto& entry = cache.at(hash);
  if (!(*entry.mutex).try_lock_shared()) {
    return nullptr;
  }
  // std::shared_lock<std::shared_mutex> lock(*entry.mutex);
  // if (entry.op == op && entry.left == left && entry.right == right) {
  //   return entry.node;
  // }
  if (!isCommutativeOperator(op) && entry.op == op && entry.left == left && entry.right == right) {
    return entry.node;
  } else {
    // and/or are commutative.
    // if (entry.op == op &&
    //     ((entry.left == left && entry.right == right))) {
    if (entry.op == op &&
        ((entry.left == left && entry.right == right) ||
         (entry.left == right && entry.right == left))) {
      return entry.node;
    }
  }
  (*entry.mutex).unlock_shared();

  return nullptr;
}

void
Cache::insert(uint32_t hash, Node* node, Node* left, Node* right, uint32_t op) {
  auto& cache = getCache(op);
  auto& entry = cache.at(hash);
  // const std::lock_guard<std::shared_mutex> lock(*entry.mutex);
  if ((*entry.mutex).try_lock()) {
    entry.node = node;
    entry.left = left;
    entry.right = right;
    entry.op = op;
    (*entry.mutex).unlock();
  }
}

bool Cache::isCommutativeOperator(Operator op) {
  // and/or/not are commutative
  return op <= 3;
}

} // namespace nanobdd