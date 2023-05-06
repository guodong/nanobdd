#include <Cache.h>
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

NodeTable* nodeTable;
Cache* cache;

static size_t varNum_;

void
init(size_t tableSize, size_t cacheSize, size_t varNum) {
  nodeTable = new NodeTable(bdd_prime_gte(tableSize));
  cache = new Cache(bdd_prime_gte(cacheSize));
  for (int id = 0; id < varNum; ++id) {
    nodeTable->createVar(id);
  }
  varNum_ = varNum;
}

Bdd
getVar(uint32_t id) {
  assert(id < varNum_);
  return nodeTable->getVar(id);
}

Bdd
getNvar(uint32_t id) {
  assert(id < varNum_);
  return nodeTable->getNvar(id);
}

inline Bdd
bddFalse() {
  return Bdd(nodeTable->falseNode());
}

inline Bdd
bddTrue() {
  return Bdd(nodeTable->trueNode());
}

} // namespace nanobdd
