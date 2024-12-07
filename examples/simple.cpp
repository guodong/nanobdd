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
  auto xyz = xy & z;
  auto xyZ = xy & !z;

  assert(xy == (xyz | xyZ));
  assert(xy != nanobdd::bddFalse());



  return 0;
}