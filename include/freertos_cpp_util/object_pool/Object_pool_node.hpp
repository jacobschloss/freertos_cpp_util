/**
 * @brief Object_pool_node
 * @author Jacob Schloss <jacob@schloss.io>
 * @copyright Copyright (c) 2018 Jacob Schloss. All rights reserved.
 * @license Licensed under the 3-Clause BSD license. See License for details
*/

#pragma once

#include "freertos_cpp_util/object_pool/Object_pool_fwd.hpp"
#include "freertos_cpp_util/object_pool/Object_pool_base.hpp"

#include <memory>
#include <utility>

template<typename T>
class Object_pool_node
{

public:

	typedef typename Object_pool_base<T>::Heap_element_T Heap_element_T;

	Object_pool_node()
	{
		m_pool_ptr = nullptr;
		m_val = nullptr;
	}

	Object_pool_node(Object_pool_base<T>* const pool, T* const val)
	{
		m_pool_ptr = pool;
		m_val = val;
	}

	Object_pool_node(const Object_pool_node& rhs)
	{
		if(this != &rhs)
		{
			*this = rhs;
		}
	}

	Object_pool_node& operator=(const Object_pool_node& rhs)
	{
		m_pool_ptr = rhs.m_pool_ptr;
		m_val = rhs.m_val;

		return *this;
	}

	~Object_pool_node()
	{
		//doing this will likely break things
		//better to leak mem?
		//although, if this is going out of scope the pool is likely also being deleted
		//so there will be no mem to leak.....
		//maybe through an exception
		//this never should be called if m_val is not null
		//if(m_val != nullptr)
		//{
			//get_val()->~T();
		//}
	}

	template<typename... Args>
	T* allocate(Args&&... args)
	{
		::new(static_cast<void*>(m_val)) T(std::forward<Args>(args)...);

		return m_val;
	}

	void deallocate()
	{
		if(m_val != nullptr)
		{
			m_val->~T();
		}
	}

	static Object_pool_node<T>* get_this_from_val_ptr(void* val_ptr)
	{
		//6.7.2.1-13. A pointer to a structure object, suitably converted, points to its initial member
		Heap_element_T* ptr = reinterpret_cast<Heap_element_T*>(val_ptr);
		return &ptr->node;
	}

	T* get_val_ptr() const
	{
		return m_val;
	}
	Object_pool_base<T>* get_pool_ptr() const
	{
		return m_pool_ptr;
	}
	
protected:

	Object_pool_base<T>* m_pool_ptr;
	T* m_val;
};

