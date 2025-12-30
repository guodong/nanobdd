#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct nanobdd_bdd_t nanobdd_bdd_t;

void nanobdd_init(size_t table_size, size_t cache_size, size_t var_num);
void nanobdd_clear();

nanobdd_bdd_t *nanobdd_bdd_false();
nanobdd_bdd_t *nanobdd_bdd_true();

nanobdd_bdd_t *nanobdd_get_var(uint32_t id);
nanobdd_bdd_t *nanobdd_get_nvar(uint32_t id);

nanobdd_bdd_t *nanobdd_bdd_clone(const nanobdd_bdd_t *bdd);
void nanobdd_bdd_free(nanobdd_bdd_t *bdd);

bool nanobdd_bdd_is_false(const nanobdd_bdd_t *bdd);
bool nanobdd_bdd_is_true(const nanobdd_bdd_t *bdd);
bool nanobdd_bdd_eq(const nanobdd_bdd_t *lhs, const nanobdd_bdd_t *rhs);

nanobdd_bdd_t *nanobdd_bdd_and(const nanobdd_bdd_t *lhs,
                               const nanobdd_bdd_t *rhs);
void nanobdd_bdd_and_assign(nanobdd_bdd_t *lhs, const nanobdd_bdd_t *rhs);

nanobdd_bdd_t *nanobdd_bdd_or(const nanobdd_bdd_t *lhs,
                              const nanobdd_bdd_t *rhs);
void nanobdd_bdd_or_assign(nanobdd_bdd_t *lhs, const nanobdd_bdd_t *rhs);

nanobdd_bdd_t *nanobdd_bdd_not(const nanobdd_bdd_t *value);

nanobdd_bdd_t *nanobdd_bdd_xor(const nanobdd_bdd_t *lhs,
                               const nanobdd_bdd_t *rhs);
void nanobdd_bdd_xor_assign(nanobdd_bdd_t *lhs, const nanobdd_bdd_t *rhs);

nanobdd_bdd_t *nanobdd_bdd_diff(const nanobdd_bdd_t *lhs,
                                const nanobdd_bdd_t *rhs);
void nanobdd_bdd_diff_assign(nanobdd_bdd_t *lhs, const nanobdd_bdd_t *rhs);

nanobdd_bdd_t *nanobdd_bdd_imp(const nanobdd_bdd_t *lhs,
                               const nanobdd_bdd_t *rhs);
void nanobdd_bdd_imp_assign(nanobdd_bdd_t *lhs, const nanobdd_bdd_t *rhs);

size_t nanobdd_num_nodes();
void nanobdd_gc();
void nanobdd_debug_nodes();

#ifdef __cplusplus
}
#endif
