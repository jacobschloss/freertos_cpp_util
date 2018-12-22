/**
 * @brief Object_pool forward ref
 * @author Jacob Schloss <jacob@schloss.io>
 * @copyright Copyright (c) 2018 Jacob Schloss. All rights reserved.
 * @license Licensed under the 3-Clause BSD license. See License for details
*/

#pragma once

#include <cstddef>

template< typename T >
class Object_pool_base;

template< typename T, size_t LEN >
class Object_pool;