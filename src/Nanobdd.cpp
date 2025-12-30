#include <Cache.h>
#include <Common.h>
#include <LockFreeCache.h>
#include <NodeTable.h>
#include <Prime.h>
#include <assert.h>
#include <nanobdd/nanobdd.h>

/**
 * @mainpage Nanobdd Documentation
 * @section intro_sec Introduction
 * Nanobdd is a high-performance thread-safe BDD library written in C++.
 * @section install_sec Installation
 * ```
 * cmake .
 * make
 * sudo make install
 * ```
 * @subsection install_dependencies Installing Dependencies
 * ```
 * sudo apt install -y libtbb-dev
 * ```
 * @subsection install_library Installing Library
 * Do somethings ...
 * @subsection install_example Installing Examples
 * Do somethings ...
 */

namespace nanobdd {

NodeTable *nodeTable;
#ifdef NANOBDD_LOCK_FREE_CACHE
LockFreeCache *cache;
#else
Cache *cache;
#endif

static size_t varNum_;

void init(size_t tableSize, size_t cacheSize, size_t varNum) {
  if (nodeTable != nullptr || cache != nullptr) {
    return;
  }
  nodeTable = new NodeTable(bdd_prime_gte(tableSize));
#ifdef NANOBDD_LOCK_FREE_CACHE
  cache = new LockFreeCache(bdd_prime_gte(cacheSize));
#else
  cache = new Cache(bdd_prime_gte(cacheSize));
#endif
  for (size_t id = 0; id < varNum; ++id) {
    nodeTable->createVar(static_cast<uint32_t>(id));
  }
  varNum_ = varNum;
}

void clear() {
#ifdef NANOBDD_LOCK_FREE_CACHE
  delete cache;
#else
  delete cache;
#endif
  cache = nullptr;
  delete nodeTable;
  nodeTable = nullptr;
  varNum_ = 0;
}

Bdd getVar(uint32_t id) {
  assert(id < varNum_);
  return nodeTable->getVar(id);
}

Bdd getNvar(uint32_t id) {
  assert(id < varNum_);
  return nodeTable->getNvar(id);
}

Bdd bddFalse() { return Bdd(nodeTable->falseNode()); }

Bdd bddTrue() { return Bdd(nodeTable->trueNode()); }

size_t numNodes() { return nodeTable->numNodes(); }

void gc() { nodeTable->gc(); }

void debugNodes() { nodeTable->debugNodes(); }

} // namespace nanobdd
