/**
 * @brief Task class with heap allocation
 * @author Jacob Schloss <jacob@schloss.io>
 * @copyright Copyright (c) 2018 Jacob Schloss. All rights reserved.
 * @license Licensed under the 3-Clause BSD license. See LICENSE for details
*/

#pragma once

#include "freertos_cpp_util/Task_base.hpp"

class Task_heap : public Task_base
{
public:
	bool launch(const char* name, const size_t StackDepth, const UBaseType_t priority);
};

template<size_t StackDepth>
class Task_heap_tmpl : public Task_heap
{
public:

    bool launch(const char* name, const UBaseType_t priority)
    {
        return Task_heap::launch(name, StackDepth, priority);
    }

protected:

};