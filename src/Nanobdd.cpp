#include <Cache.h>
#include <NodeTable.h>
#include <nanobdd.h>
#include <Prime.h>

namespace nanobdd {

NodeTable* nodeTable;
Cache* cache;

static std::vector<Bdd> vars_;
static std::vector<Bdd> nvars_;

void
init(size_t tableSize, size_t cacheSize, size_t varNum) {
  nodeTable = new NodeTable(bdd_prime_gte(tableSize));
  cache = new Cache(bdd_prime_gte(cacheSize));
  for (int id = 0; id < varNum; ++id) {
    nodeTable->createVar(id);
  }
}

Bdd getVar(uint32_t id) {
  return nodeTable->getVar(id);
}

Bdd getNvar(uint32_t id) {
  return nodeTable->getNvar(id);
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
