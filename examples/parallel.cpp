// include the nanobdd header file
#include <assert.h>
#include <nanobdd/nanobdd.h>
#include <algorithm>
#include <execution>

int
main(int argc, char** argv) {
  // init nanobdd with node table size, cache size, and the number of variables
  nanobdd::init(1000, 1000, 3);

  // get the three variables
  auto x = nanobdd::getVar(0);
  auto y = nanobdd::getVar(1);
  auto z = nanobdd::getVar(2);

  std::vector<nanobdd::Bdd> bdds = {x, y, z};

  auto xy = x & y;

  std::for_each(std::execution::par, bdds.begin(), bdds.end(), [&](auto bdd) {
    bdd | xy;
  });

  return 0;
}