#include <nanobdd/capi.h>
#include <nanobdd/nanobdd.h>

struct nanobdd_bdd_t {
  nanobdd::Bdd bdd;
};

extern "C" {

void nanobdd_init(size_t table_size, size_t cache_size, size_t var_num) {
  nanobdd::init(table_size, cache_size, var_num);
}

void nanobdd_clear() { nanobdd::clear(); }

nanobdd_bdd_t *nanobdd_bdd_false() {
  return new nanobdd_bdd_t{nanobdd::bddFalse()};
}

nanobdd_bdd_t *nanobdd_bdd_true() {
  return new nanobdd_bdd_t{nanobdd::bddTrue()};
}

nanobdd_bdd_t *nanobdd_get_var(uint32_t id) {
  return new nanobdd_bdd_t{nanobdd::getVar(id)};
}

nanobdd_bdd_t *nanobdd_get_nvar(uint32_t id) {
  return new nanobdd_bdd_t{nanobdd::getNvar(id)};
}

nanobdd_bdd_t *nanobdd_bdd_clone(const nanobdd_bdd_t *bdd) {
  if (bdd == nullptr) {
    return nullptr;
  }
  return new nanobdd_bdd_t{bdd->bdd};
}

void nanobdd_bdd_free(nanobdd_bdd_t *bdd) { delete bdd; }

bool nanobdd_bdd_is_false(const nanobdd_bdd_t *bdd) {
  if (bdd == nullptr) {
    return false;
  }
  return bdd->bdd.isFalse();
}

bool nanobdd_bdd_is_true(const nanobdd_bdd_t *bdd) {
  if (bdd == nullptr) {
    return false;
  }
  return bdd->bdd.isTrue();
}

bool nanobdd_bdd_eq(const nanobdd_bdd_t *lhs, const nanobdd_bdd_t *rhs) {
  if (lhs == nullptr || rhs == nullptr) {
    return lhs == rhs;
  }
  return lhs->bdd == rhs->bdd;
}

nanobdd_bdd_t *nanobdd_bdd_and(const nanobdd_bdd_t *lhs,
                               const nanobdd_bdd_t *rhs) {
  if (lhs == nullptr || rhs == nullptr) {
    return nullptr;
  }
  return new nanobdd_bdd_t{lhs->bdd & rhs->bdd};
}

void nanobdd_bdd_and_assign(nanobdd_bdd_t *lhs, const nanobdd_bdd_t *rhs) {
  if (lhs == nullptr || rhs == nullptr) {
    return;
  }
  lhs->bdd &= rhs->bdd;
}

nanobdd_bdd_t *nanobdd_bdd_or(const nanobdd_bdd_t *lhs,
                              const nanobdd_bdd_t *rhs) {
  if (lhs == nullptr || rhs == nullptr) {
    return nullptr;
  }
  return new nanobdd_bdd_t{lhs->bdd | rhs->bdd};
}

void nanobdd_bdd_or_assign(nanobdd_bdd_t *lhs, const nanobdd_bdd_t *rhs) {
  if (lhs == nullptr || rhs == nullptr) {
    return;
  }
  lhs->bdd |= rhs->bdd;
}

nanobdd_bdd_t *nanobdd_bdd_not(const nanobdd_bdd_t *value) {
  if (value == nullptr) {
    return nullptr;
  }
  return new nanobdd_bdd_t{!value->bdd};
}

nanobdd_bdd_t *nanobdd_bdd_xor(const nanobdd_bdd_t *lhs,
                               const nanobdd_bdd_t *rhs) {
  if (lhs == nullptr || rhs == nullptr) {
    return nullptr;
  }
  return new nanobdd_bdd_t{lhs->bdd ^ rhs->bdd};
}

void nanobdd_bdd_xor_assign(nanobdd_bdd_t *lhs, const nanobdd_bdd_t *rhs) {
  if (lhs == nullptr || rhs == nullptr) {
    return;
  }
  lhs->bdd ^= rhs->bdd;
}

nanobdd_bdd_t *nanobdd_bdd_diff(const nanobdd_bdd_t *lhs,
                                const nanobdd_bdd_t *rhs) {
  if (lhs == nullptr || rhs == nullptr) {
    return nullptr;
  }
  return new nanobdd_bdd_t{lhs->bdd - rhs->bdd};
}

void nanobdd_bdd_diff_assign(nanobdd_bdd_t *lhs, const nanobdd_bdd_t *rhs) {
  if (lhs == nullptr || rhs == nullptr) {
    return;
  }
  lhs->bdd -= rhs->bdd;
}

nanobdd_bdd_t *nanobdd_bdd_imp(const nanobdd_bdd_t *lhs,
                               const nanobdd_bdd_t *rhs) {
  if (lhs == nullptr || rhs == nullptr) {
    return nullptr;
  }
  return new nanobdd_bdd_t{lhs->bdd >> rhs->bdd};
}

void nanobdd_bdd_imp_assign(nanobdd_bdd_t *lhs, const nanobdd_bdd_t *rhs) {
  if (lhs == nullptr || rhs == nullptr) {
    return;
  }
  lhs->bdd >>= rhs->bdd;
}

size_t nanobdd_num_nodes() { return nanobdd::numNodes(); }

void nanobdd_gc() { nanobdd::gc(); }

void nanobdd_debug_nodes() { nanobdd::debugNodes(); }

} // extern "C"
