/**
 * @brief Heap allocated recursive mutex
 * @author Jacob Schloss <jacob@schloss.io>
 * @copyright Copyright (c) 2019 Jacob Schloss. All rights reserved.
 * @license Licensed under the 3-Clause BSD license. See LICENSE for details
*/

#pragma once

#include "freertos_cpp_util/Mutex_base.hpp"

class Mutex_heap_recursive : public Mutex_base
{
public:
	Mutex_heap_recursive()
	{
		m_mutex = xSemaphoreCreateRecursiveMutex();
	}
	~Mutex_heap_recursive() override
	{

	}
protected:
	
};
