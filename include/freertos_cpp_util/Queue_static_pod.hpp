/**
 * @brief * @brief Stack allocated queue class for POD objects
 * @author Jacob Schloss <jacob@schloss.io>
 * @copyright Copyright (c) 2018 Jacob Schloss. All rights reserved.
 * @license Licensed under the 3-Clause BSD license. See License for details
*/

#pragma once

#include "freertos_cpp_util/Queue_base.hpp"

#include <array>
#include <type_traits>

template<typename T, size_t LEN>
class Queue_static_pod : public Queue_template_base_pod<T>
{
public:

	static_assert(std::is_pod<T>::value, "T must be POD");

	Queue_static_pod()
	{
		this->m_queue = xQueueCreateStatic(LEN, sizeof(T), reinterpret_cast<uint8_t*>(m_buf.data()), &m_queue_buf);
	}

protected:
	StaticQueue_t m_queue_buf;

	std::array<T, LEN> m_buf;
};
