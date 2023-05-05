#pragma once

#include <nanobdd/Bdd.h>

#include <stdint.h>
#include <cstddef>

/**
 * @mainpage Nanobdd Documentation
 * @section intro_sec Introduction
 * Nanobdd is a high-performance thread-safe BDD library written in C++.
 * @section install_sec Installation
 * ```
 * cmake
 * make
 * sudo make install
 * ```
 * @subsection install_dependencies Installing Dependencies
 * Do somethings ...
 * @subsection install_library Installing Library
 * Do somethings ...
 * @subsection install_example Installing Examples
 * Do somethings ...
 */

namespace nanobdd {

/**
 * @brief Init nanobdd, this should only be called once.
 *
 * @param tableSize The node table size
 * @param cacheSize The cache size
 * @param varNum The number of variables
 */
void init(size_t tableSize, size_t cacheSize, size_t varNum);

Bdd bddFalse();
Bdd bddTrue();

Bdd getVar(uint32_t id);
Bdd getNvar(uint32_t id);

} // namespace nanobdd
