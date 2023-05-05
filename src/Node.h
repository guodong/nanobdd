#pragma once

#include <stdint.h>

namespace nanobdd {

struct Node {
  uint32_t level;
  Node* low;
  Node* high;
};

} // namespace nanobdd