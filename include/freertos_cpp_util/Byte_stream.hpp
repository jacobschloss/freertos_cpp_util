/**
 * @brief Thread-safe byte stream
 * @author Jacob Schloss <jacob@schloss.io>
 * @copyright Copyright (c) 2018 Jacob Schloss. All rights reserved.
 * @license Licensed under the 3-Clause BSD license. See License for details
*/

#pragma once

#include "freertos_cpp_util/Mutex_static.hpp"
#include "freertos_cpp_util/BSema_static.hpp"

#include <array>
#include <list>
#include <chrono>
#include <mutex>

#include <cstdint>
#include <cstddef>

//manages an asynchronous inter-thread byte stream with timeouts
class Byte_stream
{
public:

	Byte_stream()
	{

	}

	virtual ~Byte_stream()
	{
		
	}

	Byte_stream(const Byte_stream& rhs)
	{
		if(this == &rhs)
		{
			return;
		}

		*this = rhs;
	}

	Byte_stream& operator=(const Byte_stream& rhs)
	{
		{
			std::lock_guard<Mutex_static> lock1(m_read_buf_mutex);
			std::lock_guard<Mutex_static> lock2(rhs.m_read_buf_mutex);

			m_read_buf = rhs.m_read_buf;
		}
		{
			std::lock_guard<Mutex_static> lock1(m_write_buf_mutex);
			std::lock_guard<Mutex_static> lock2(rhs.m_write_buf_mutex);

			m_write_buf = rhs.m_write_buf;
		}

		return *this;
	}

	size_t read_some(const uint8_t* buf, const size_t read_max);

	template< class Rep, class Period >
	size_t read_some_for(uint8_t* const buf, const size_t read_max, const std::chrono::duration<Rep,Period>& duration)
	{
		std::chrono::milliseconds duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
		return read_some_for_ticks(buf, read_max, pdMS_TO_TICKS(duration_ms.count()));
	}

	size_t read_some_for_ticks(uint8_t* const buf, const size_t read_max, const TickType_t xTicksToWait);


	size_t write_some(const uint8_t* buf, const size_t write_max);

	template< class Rep, class Period >
	size_t write_some_for(const uint8_t* buf, const size_t write_max, const std::chrono::duration<Rep,Period>& duration)
	{
		std::chrono::milliseconds duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
		return write_some_for_ticks(buf, write_max, pdMS_TO_TICKS(duration_ms.count()));
	}

	size_t write_some_for_ticks(const uint8_t* buf, const size_t write_max, const TickType_t xTicksToWait);


protected:

	std::list<uint8_t> m_read_buf;
	mutable Mutex_static m_read_buf_mutex;

	std::list<uint8_t> m_write_buf;
	mutable Mutex_static m_write_buf_mutex;

	//when reading, take this to wait for new data
	//writers will give it
	BSema_static m_write_activity;

	//when writing take this to wait for free space
	//readers will give it
	BSema_static m_read_activity;

};
