/**
 * @brief Stack allocated queue class for full C++ objects
 * @author Jacob Schloss <jacob@schloss.io>
 * @copyright Copyright (c) 2018 Jacob Schloss. All rights reserved.
 * @license Licensed under the 3-Clause BSD license. See LICENSE for details
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
		while(m_alloc_queue.pop_front(&ptr))
		{
			m_pool.free(ptr);
		}
	}

	bool pop_front(T* const item) override
	{
		return pop_front(item, 0);
	}

	bool pop_front(T* const item, const TickType_t xTicksToWait) override
	{
		//do we have one?
		T* ptr = nullptr;
		if(!m_alloc_queue.pop_front(&ptr, xTicksToWait))
		{
			return false;
		}

		//use move, it might be optimized
		*item = std::move(*ptr);

		//free our internal copy
		m_pool.free(ptr);

		return true;
	}

	bool push_back(const T& item) override
	{
		return push_back(item, 0);
	}

	bool push_back(const T& item, const TickType_t xTicksToWait) override
	{
		//calls copy constructor if there is a free node
		T* const ptr = m_pool.try_allocate_for_ticks(xTicksToWait, item);

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

	bool push_front(const T& item) override
	{
		return push_front(item, 0);
	}

	bool push_front(const T& item, const TickType_t xTicksToWait) override
	{
		//calls copy constructor if there is a free node
		T* const ptr = m_pool.try_allocate_for_ticks(xTicksToWait, item);

		if(!ptr)
		{
			return false;
		}

		//stash our ref
		if(!m_alloc_queue.push_front(ptr, 0))
		{
			return false;
		}

		return true;
	}

	bool push_front_isr(const T& item)
	{
		//Object_pool is not isr safe yet
		return false;
	}

	bool push_front_isr(const T& item, BaseType_t* const pxHigherPriorityTaskWoken)
	{
		//Object_pool is not isr safe yet
		return false;
	}

	bool push_back_isr(const T& item)
	{
		//Object_pool is not isr safe yet
		return false;
	}

	bool push_back_isr(const T& item, BaseType_t* const pxHigherPriorityTaskWoken)
	{
		//Object_pool is not isr safe yet
		return false;
	}

protected:

	Object_pool<T, LEN> m_pool;

	Queue_static_pod<T*, LEN> m_alloc_queue;
};
