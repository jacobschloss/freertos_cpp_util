# freertos_cpp_util

A collection of C++ wrappers and utility code for FreeRTOS.

Features
 * Wrappers for tasks, forming thread objects
 * Wrappers for queues
    * Basic wrapper for POD types only
    * Full featured version using object pool for full C++ objects
       * Allows arbitrary alignment
       * Calls constructors, destructors
 * Threading primitives
    * Wrappers for mutex, binary semaphore, counting semaphore
    * A condition variable for FreeRTOS, implemented with a binary semaphore and queue
 * A C++ safe object pool
    * Usable as a simple heap
    * Calls constructor & destructor
    * Supports arbitrary alignment requirements (ie, alignas specifier)
    * Premptable
    * Can block for configurable amount of time
 * A C++11 style allocator
    * Supports types with wider alignment than the default portBYTE_ALIGNMENT  (ie, alignas specifier)
 * Some support for chrono types
 * Utility code
    * A Non_copyable class
    * An intrusive singly linked list
    * Scoped ISR disable, scheduler disable
 * CMake build script

# License

Licensed under the terms of the 3-Clause BSD license. See LICENSE for details.

# Copyright

Copyright (c) 2018 Jacob Schloss