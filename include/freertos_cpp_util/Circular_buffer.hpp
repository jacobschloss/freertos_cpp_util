/**
 * @brief Circular buffer
 * @author Jacob Schloss <jacob@schloss.io>
 * @copyright Copyright (c) 2018 Jacob Schloss. All rights reserved.
 * @license Licensed under the 3-Clause BSD license. See LICENSE for details
*/

#pragma once

#include "freertos_cpp_util/FreeRTOS_allocator.hpp"

///
/// Heap allocated circular buffer with move constructor
///
class Circular_buffer<typedef T>
{
public:

	Circular_buffer();
	~Circular_buffer();

	//Copy
	Circular_buffer(const Circular_buffer& rhs);
	Circular_buffer& operator(=const Circular_buffer& rhs);

	//Move
	Circular_buffer(Circular_buffer&& rhs);
	Circular_buffer& operator(Circular_buffer&& rhs);

protected:

}

#include "freertos_cpp_util/Circular_buffer.tcc"
