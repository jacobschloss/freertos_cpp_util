/**
 * @brief Object_pool_base
 * @author Jacob Schloss <jacob@schloss.io>
 * @copyright Copyright (c) 2018 Jacob Schloss. All rights reserved.
 * @license Licensed under the 3-Clause BSD license. See License for details
*/

#pragma once

#include "freertos_cpp_util/Non_copyable.hpp"

#include "freertos_cpp_util/object_pool/Object_pool_node.hpp"

#include <type_traits>
#include <utility>

template< typename T>
class Object_pool_base : private Non_copyable
{
public:

	typedef std::aligned_storage_t<sizeof(T), alignof(T)> Aligned_T;
	typedef Object_pool_node<T> Node_T;

	//allows getting a node* from a val*
	//since struct* is first element*
	struct Heap_element_T
	{
		Aligned_T val;
		Node_T node;
	};

	Object_pool_base() = default;
	virtual ~Object_pool_base()
	{

	}

	virtual void deallocate(T* const ptr) = 0;
	virtual void deallocate(Node_T* const node) = 0;

	//convinence function, if you don't know or care which pool owns it
	//will lookup the correct pool to return to
	//slightly slower than pool direct deallocation
	static void free(T* const ptr)
	{
		Node_T* node = Node_T::get_this_from_val_ptr(ptr);

		Object_pool_base<T>* pool = node->get_pool_ptr();

		pool->deallocate(node);
	}

protected:

};