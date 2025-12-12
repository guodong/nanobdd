// include the nanobdd header file
#include <algorithm>
#include <assert.h>
#include <execution>
#include <nanobdd/nanobdd.h>
#include <vector>

int main(int argc, char **argv) {
  // init nanobdd with node table size, cache size, and the number of
  // variables
  nanobdd::init(1000, 1000, 3);

  // get the three variables
  auto x = nanobdd::getVar(0);
  auto y = nanobdd::getVar(1);
  auto z = nanobdd::getVar(2);

  std::vector<nanobdd::Bdd> bdds = {x, y, z};

  auto xy = x & y;

#if defined(__cpp_lib_parallel_algorithm)
  std::for_each(std::execution::par, bdds.begin(), bdds.end(),
                [&](auto bdd) { bdd | xy; });
#else
  // Fall back to sequential execution if the standard parallel algorithms
  // implementation is unavailable (e.g., older libc++ on macOS).
  std::for_each(bdds.begin(), bdds.end(), [&](auto bdd) { bdd | xy; });
#endif

  return 0;
}
