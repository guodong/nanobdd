#pragma once

#include <nanobdd/Bdd.h>

#include <cstddef>
#include <stdint.h>

namespace nanobdd {

/**
 * @brief Init nanobdd, this should only be called once.
 *
 * @param tableSize The node table size
 * @param cacheSize The cache size
 * @param varNum The number of variables
 */
void init(size_t tableSize, size_t cacheSize, size_t varNum);

/**
 * @brief Get the false terminal node as a Bdd
 *
 * @return Bdd
 */
Bdd bddFalse();

/**
 * @brief Get the true terminal node as a Bdd
 *
 * @return Bdd
 */
Bdd bddTrue();

/**
 * @brief Get the id-th variable
 *
 * @param id The id/level of the variable, id must be less than varNum
 * @return Bdd
 */
Bdd getVar(uint32_t id);

/**
 * @brief Get the id-th negative variable
 *
 * @param id The id/level of the variable, id must be less than varNum
 * @return Bdd
 */
Bdd getNvar(uint32_t id);

size_t numNodes();

void gc();

void debugNodes();

} // namespace nanobdd
