/**
 * @author Jacob Schloss <jacob@schloss.io>
 * @copyright Copyright (c) 2023 Jacob Schloss. All rights reserved.
 * @license Licensed under the 3-Clause BSD license. See LICENSE for details
*/

#pragma once

#include "FreeRTOS.h"
#include "stream_buffer.h"

#include <cstring>

#include <chrono>

class Stream_buffer
{
public:
	Stream_buffer()
	{
		m_handle = nullptr;
		memset(&m_inst, 0, sizeof(m_inst));

		m_buf = nullptr;
		m_len = 0;
	}
	~Stream_buffer()
	{
		reset();
	}

	bool create(uint8_t* const buf, const size_t len)
	{
		if(m_handle)
		{
			reset();
		}

		m_buf = buf;
		m_len = len;

		m_handle = xStreamBufferCreateStatic(m_len, 1, m_buf, &m_inst);

		return m_handle != nullptr;
	}

	// bool create(const size_t len)
	// {
	// 	if(m_handle)
	// 	{
	// 		reset();
	// 	}
	//
	// 	m_buf = nullptr;
	// 	m_len = len;
	//
	// 	m_handle = xStreamBufferCreate(m_len, 1);
	//
	// 	return m_handle != nullptr;
	// }

	void reset()
	{
		vStreamBufferDelete(m_handle);
		m_handle = nullptr;
		memset(&m_inst, 0, sizeof(m_inst));

		m_buf = nullptr;
		m_len = 0;
	}

	bool set_trigger_level(const size_t len)
	{
		return xStreamBufferSetTriggerLevel(m_handle, len) == pdTRUE;
	}

	template< class Rep, class Period >
	size_t write(uint8_t const * const data, const size_t len, const std::chrono::duration<Rep,Period>& duration)
	{
		const std::chrono::milliseconds duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
		return write(data, len, pdMS_TO_TICKS(duration_ms.count()));
	}

	size_t write(uint8_t const * const data, const size_t len, const TickType_t xTicksToWait)
	{
		return xStreamBufferSend(m_handle, data, len, xTicksToWait);
	}

	size_t write_isr(const uint8_t data, BaseType_t* const pxHigherPriorityTaskWoken)
	{
		return xStreamBufferSendFromISR(m_handle, &data, 1, pxHigherPriorityTaskWoken);
	}

	size_t write_isr(uint8_t const  * const data, const size_t len, BaseType_t* const pxHigherPriorityTaskWoken)
	{
		return xStreamBufferSendFromISR(m_handle, data, len, pxHigherPriorityTaskWoken);
	}

	template< class Rep, class Period >
	size_t read(uint8_t * const data, const size_t len, const std::chrono::duration<Rep,Period>& duration)
	{
		const std::chrono::milliseconds duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
		return read(data, len, pdMS_TO_TICKS(duration_ms.count()));
	}

	size_t read(uint8_t * const data, const size_t len, const TickType_t xTicksToWait)
	{
		return xStreamBufferReceive(m_handle, data, len, xTicksToWait);
	}

	size_t read_isr(uint8_t * const data, const size_t len, BaseType_t* const pxHigherPriorityTaskWoken)
	{
		return xStreamBufferReceiveFromISR(m_handle, data, len, pxHigherPriorityTaskWoken);
	}

	bool empty()
	{
		return xStreamBufferIsEmpty(m_handle) == pdTRUE;
	}

	bool full()
	{
		return xStreamBufferIsFull(m_handle) == pdTRUE;
	}

	size_t size()
	{
		return xStreamBufferBytesAvailable(m_handle);
	}

	size_t reserve()
	{
		return xStreamBufferSpacesAvailable(m_handle);
	}

	size_t capacity() const
	{
		return m_len;
	}

protected:
	StreamBufferHandle_t m_handle;
	StaticStreamBuffer_t m_inst;

	uint8_t* m_buf;
	size_t   m_len;
};
