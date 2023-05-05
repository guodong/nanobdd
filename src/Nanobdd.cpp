#include <Cache.h>
#include <NodeTable.h>
#include <nanobdd.h>
#include <Prime.h>

namespace nanobdd {

NodeTable* nodeTable;
Cache* cache;

void
init(size_t tableSize, size_t cacheSize, size_t varNum) {
  nodeTable = new NodeTable(bdd_prime_gte(tableSize));
  cache = new Cache(bdd_prime_gte(cacheSize));
}

Bdd
bddFalse() {
  return Bdd(nodeTable->falseNode());
}

Bdd
bddTrue() {
  return Bdd(nodeTable->trueNode());
}

} // namespace nanobdd
