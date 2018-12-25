# freertos_cpp_util

A collection of C++ wrappers and utility code for FreeRTOS.

Features
 * Wrappers for tasks, forming thread objects
 * Wrappers for queus, POD only
 * Wrappers threading primitives
 * A C++ safe object pool
    * usable as a simple heap
    * calls constructor & destructor
    * premptable
    * can block for configurable amount of time
 * A C++11 style allocator
    * Supports types with wider alignment than the default portBYTE_ALIGNMENT
 * Some support for chrono types
 * CMake build script

# License

Licensed under the terms of the 3-Clause BSD license. See LICENSE for details.

# Copyright

Copyright (c) 2018 Jacob Schloss