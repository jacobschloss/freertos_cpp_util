/**
 * @brief Object_pool
 * @author Jacob Schloss <jacob@schloss.io>
 * @copyright Copyright (c) 2018 Jacob Schloss. All rights reserved.
 * @license Licensed under the 3-Clause BSD license. See License for details
*/

#pragma once

#include "freertos_cpp_util/Queue_static_pod.hpp"

#include "freertos_cpp_util/object_pool/Object_pool_node.hpp"
#include "freertos_cpp_util/object_pool/Object_pool_base.hpp"

#include <type_traits>
#include <utility>

template< typename T, size_t LEN >
class Object_pool : public Object_pool_base<T>
{

public:

	using typename Object_pool_base<T>::Aligned_T;
	using typename Object_pool_base<T>::Node_T;
	using typename Object_pool_base<T>::Heap_element_T;

	Object_pool()
	{
		for(size_t i = 0; i < LEN; i++)
		{
			Heap_element_T* const mem_ptr = &m_mem_node_pool[i];

			mem_ptr->node = Node_T(this, reinterpret_cast<T*>(&mem_ptr->val));

			m_free_nodes.push_back(&mem_ptr->node);
		}
	}

	// template<typename... Args>
	// T* allocate(const TickType_t xTicksToWait, Args&&... args)
	// {
	// 	Node_T* node = nullptr;
	// 	if(!m_free_nodes.pop_front(&node, xTicksToWait))
	// 	{
	// 		return false;
	// 	}

	// 	return node->allocate(std::forward<Args>(args)...);
	// }

	template<typename... Args>
	T* allocate(Args&&... args)
	{
		Node_T* node = nullptr;
		if(!m_free_nodes.pop_front(&node))
		{
			return nullptr;
		}

		T* const val_ptr = node->allocate(std::forward<Args>(args)...);

		return val_ptr;
	}

	//convinence function, if you don't know or care which pool owns it
	//will lookup the correct pool to return to
	//slightly slower than pool direct deallocation
	static void free(T* const ptr)
	{
		Node_T* node = Node_T::get_this_from_val_ptr(ptr);

		Object_pool_base<T>* pool = node->get_pool_ptr();

		pool->deallocate(node);
	}

	class Node_T_deleter
	{
	public:
		void operator()(T* ptr) const
		{
			Node_T* node = Node_T::get_this_from_val_ptr(ptr);
			Object_pool_base<T>* pool = node->get_pool_ptr();

			pool->deallocate(node);
		}
	};

	typedef std::unique_ptr<T, Node_T_deleter> unique_node_ptr;

	// template<typename... Args>
	// unique_node_ptr allocate_unique(const TickType_t xTicksToWait, Args&&... args)
	// {
	// 	T* val = allocate(xTicksToWait, std::forward<Args>(args)...);

	// 	return unique_node_ptr(val);
	// }

	template<typename... Args>
	unique_node_ptr allocate_unique(Args&&... args)
	{
		T* val = allocate(std::forward<Args>(args)...);

		return unique_node_ptr(val);
	}

	//the "best" deallocator
	//node must belong to this pool
	void deallocate(Node_T* const node) override
	{
		node->deallocate();

		// Object_pool_base<T>* pool = node->get_pool();
		// if(pool != this)
		// {
		// 	//you tried to delete a foreign node
		// 	//that is bad
		// }

		if(!m_free_nodes.push_front(node))
		{
			//this should never fail
			//very bad if this fails
		}
	}

	//look up the node based on the ptr
	//ptr must belong to this pool
	void deallocate(T* const ptr) override
	{
		Node_T* node = Node_T::get_this_from_val_ptr(ptr);
		
		deallocate(node);
	}

protected:

	//heap element: node and aligned storage
	std::array<Heap_element_T, LEN> m_mem_node_pool;

	//tracks free nodes
	Queue_static_pod<Node_T*, LEN> m_free_nodes;
};