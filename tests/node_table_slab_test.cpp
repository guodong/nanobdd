#include <nanobdd/Bdd.h>
#include <nanobdd/nanobdd.h>

#include <cassert>
#include <iostream>
#include <vector>

using namespace nanobdd;

int main() {
  constexpr size_t tableSize = 5; // intentionally tiny to force growth
  constexpr size_t cacheSize = 5;
  constexpr size_t varNum = 3;

  init(tableSize, cacheSize, varNum);

  const auto baseline = numNodes();

  // Trigger many unique nodes to push past the initial slab capacity and hash
  // bucket count. If growth fails, we would crash or get nullptr derefs.
  std::vector<Bdd> arena;
  arena.reserve(200);
  for (int i = 0; i < 200; ++i) {
    auto a = getVar(i % varNum);
    auto b = getVar((i + 1) % varNum);
    auto c = getVar((i + 2) % varNum);
    arena.push_back((a ^ b) | (a & c));
  }

  assert(numNodes() > baseline + 10 &&
         "Slab growth / maintenance path should allow many allocations");

  // Release BDDs so their roots are deref'ed, then run GC to reclaim.
  arena.clear();
  gc();

  const auto afterGc = numNodes();
  const auto maxReachable = 2 + 2 * varNum; // terminals + vars + nvars
  assert(afterGc <= maxReachable &&
         "GC should reclaim unreachable nodes into free list");

  // Allocate again; should succeed via free list without unbounded growth.
  const auto beforeAlloc = numNodes();
  auto d = getVar(0) ^ getVar(1);
  (void)d;
  assert(numNodes() <= beforeAlloc + 4 &&
         "Re-allocation should use reclaimed nodes or a small growth");

  std::cout << "node_table_slab_test passed" << std::endl;
  return 0;
}
