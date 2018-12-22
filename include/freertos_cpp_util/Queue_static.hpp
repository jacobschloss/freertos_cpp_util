/**
 * @brief Stack allocated queue class for full C++ objects
 * @author Jacob Schloss <jacob@schloss.io>
 * @copyright Copyright (c) 2018 Jacob Schloss. All rights reserved.
 * @license Licensed under the 3-Clause BSD license. See License for details
*/

#pragma once

#include "freertos_cpp_util/object_pool/Object_pool.hpp"

#include "FreeRTOS.h"
#include "task.h"

#include <array>

template<typename T, size_t LEN>
class Queue_static : public Queue_template_base<T>
{
public:

	Queue_static()
	{
		
	}
//	~Queue_static() override
//	{
//
//	}

	bool push_back(const T& item)
	{
		return push_back(item, 0);
	}

	bool push_back(const T& item, const TickType_t xTicksToWait)
	{
		T* ptr = m_pool.try_allocate(xTicksToWait, item);

		if(!m_alloc_queue.push_back(ptr, 0))
		{
			return false;
		}

		return true;
	}

protected:

	//
	Object_pool<T, LEN> m_pool;

	Queue_static_pod<T*, LEN> m_alloc_queue;
};
