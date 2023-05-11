#pragma once

#define NANOBDD_LOCK_FREE_BUCKET
#define NANOBDD_LOCK_FREE_CACHE

namespace nanobdd {

enum Operator { AND = 0, OR = 1, NOT = 2, XOR = 3, DIFF = 4, IMP = 5 };

}