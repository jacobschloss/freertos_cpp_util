/**
 * @brief Stack allocated queue class for full C++ objects
 * @author Jacob Schloss <jacob@schloss.io>
 * @copyright Copyright (c) 2018 Jacob Schloss. All rights reserved.
 * @license Licensed under the 3-Clause BSD license. See License for details
*/

#pragma once

#include "freertos_cpp_util/Queue_static_pod.hpp"
#include "freertos_cpp_util/object_pool/Object_pool.hpp"

template<typename T, size_t LEN>
class Queue_static : public Queue_template_base<T>
{
public:

	Queue_static()
	{
		
	}
	~Queue_static() override
	{
		T* ptr = nullptr;
		while(m_alloc_queue.pop_front(ptr))
		{
			m_pool.free(ptr);
		}
	}

	bool push_back(const T& item) override
	{
		return push_back(item, 0);
	}

	bool push_back(const T& item, const TickType_t xTicksToWait) override
	{
		//calls copy constructor if there is a free node
		T* ptr = m_pool.try_allocate(xTicksToWait, item);

		if(!ptr)
		{
			return false;
		}

		//stash our ref
		if(!m_alloc_queue.push_back(ptr, 0))
		{
			return false;
		}

		return true;
	}

	bool pop_front(T* const item) override
	{
		return pop_front(item, 0);
	}

	bool pop_front(T* const item, const TickType_t xTicksToWait) override
	{
		//do we have one?
		T* ptr = nullptr;
		if(!m_alloc_queue.pop_front(ptr, xTicksToWait))
		{
			return false;
		}

		//copy over
		*item = ptr;

		//free our internal copy
		m_pool.free(ptr);

		return true;
	}

protected:

	//
	Object_pool<T, LEN> m_pool;

	Queue_static_pod<T*, LEN> m_alloc_queue;
};
