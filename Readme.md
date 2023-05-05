# Nanobdd
First-ever high-performance thread-safe BDD (Binary Decision Diagrams) library.

# Basic usage
A simple c++ code to use nanobdd is as follows:
```
// include the nanobdd header file
#include <nanobdd/nanobdd.h>
#include <assert.h>

int main(int argc, char** argv) {
  // init nanobdd with node table size, cache size, and the number of variables
  nanobdd::init(1000, 1000, 3);

  // get the three variables
  auto x = nanobdd::getVar(0);
  auto y = nanobdd::getVar(1);
  auto z = nanobdd::getVar(2);

  // do magic using c++ operators
  auto xy = x & y;
  auto xyz = xy | z;
  auto xyzZ = xyz | !z;

  assert(xy == xyzZ);
  assert(xy != nanobdd::bddFalse());

  return 0;
}
```

Compile the above code by:
```
g++ [file] -lnanobdd
```

# Thread-safe concurrency
The most powerful feature of nanobdd is that it is thread-safe, that means one can safely perform any bdd operations in different threads, nanobdd will handle all underlay data contensions.
An example for using C++17 parallel STL:
```
std::for_each(
  std::execution::par,
  somebdds.begin(),
  somebdds.end(),
  [&](auto bdd) {
    // operate your bdd here
  });
```

# Performance
We have compared nanobdd with other librarys including Buddy, JDD and Sylvan in a network verification project on a 40 CPU cores server. Typically, nanobdd is 2~10x faster than others.