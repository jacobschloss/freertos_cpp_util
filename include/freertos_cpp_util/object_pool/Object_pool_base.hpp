/**
 * @brief Object_pool_base
 * @author Jacob Schloss <jacob@schloss.io>
 * @copyright Copyright (c) 2018 Jacob Schloss. All rights reserved.
 * @license Licensed under the 3-Clause BSD license. See License for details
*/

#pragma once

#include "freertos_cpp_util/object_pool/Object_pool_fwd.hpp"

#include <type_traits>
#include <utility>

template< typename T>
class Object_pool_base
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

protected:

};