/**
 * @author Jacob Schloss <jacob@schloss.io>
 * @copyright Copyright (c) 2023 Jacob Schloss. All rights reserved.
 * @license Licensed under the 3-Clause BSD license. See LICENSE for details
*/

#pragma once

#include "FreeRTOS.h"
#include "message_buffer.h"

#include <cstring>

#include <chrono>

class Message_buffer
{
public:
	Message_buffer()
	{
		m_handle = nullptr;
		m_len = 0;
	}
	virtual ~Message_buffer()
	{
		reset();
	}

	virtual bool create(const size_t len)
	{
		if(m_handle)
		{
			reset();
		}
	
		m_len = len;
	
		m_handle = xMessageBufferCreate(m_len);
	
		return m_handle != nullptr;
	}

	virtual bool create(uint8_t* const buf, const size_t len)
	{
		return false;
	}

	virtual void reset()
	{
		if(m_handle)
		{
			vMessageBufferDelete(m_handle);
			m_handle = nullptr;
		}
		m_len = 0;
	}

	template< class Rep, class Period >
	size_t write(uint8_t const * const data, const size_t len, const std::chrono::duration<Rep,Period>& duration)
	{
		const std::chrono::milliseconds duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
		return write(data, len, pdMS_TO_TICKS(duration_ms.count()));
	}

	size_t write(uint8_t const * const data, const size_t len, const TickType_t xTicksToWait)
	{
		return xMessageBufferSend(m_handle, data, len, xTicksToWait);
	}

	size_t write_isr(uint8_t const  * const data, const size_t len, BaseType_t* const pxHigherPriorityTaskWoken)
	{
		return xMessageBufferSendFromISR(m_handle, data, len, pxHigherPriorityTaskWoken);
	}

	template< class Rep, class Period >
	size_t read(uint8_t * const data, const size_t len, const std::chrono::duration<Rep,Period>& duration)
	{
		const std::chrono::milliseconds duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
		return read(data, len, pdMS_TO_TICKS(duration_ms.count()));
	}

	size_t read(uint8_t * const data, const size_t len, const TickType_t xTicksToWait)
	{
		return xMessageBufferReceive(m_handle, data, len, xTicksToWait);
	}

	size_t read_isr(uint8_t * const data, const size_t len, BaseType_t* const pxHigherPriorityTaskWoken)
	{
		return xMessageBufferReceiveFromISR(m_handle, data, len, pxHigherPriorityTaskWoken);
	}

	bool empty()
	{
		return xMessageBufferIsEmpty(m_handle) == pdTRUE;
	}

	bool full()
	{
		return xMessageBufferIsFull(m_handle) == pdTRUE;
	}

	size_t size()
	{
		return capacity() - reserve();
	}

	size_t reserve()
	{
		return xMessageBufferSpacesAvailable(m_handle);
	}

	size_t capacity() const
	{
		return m_len;
	}

protected:
	MessageBufferHandle_t m_handle;
	size_t   m_len;
};

class Message_buffer_static : public Message_buffer
{
public:

	Message_buffer_static()
	{
		memset(&m_inst, 0, sizeof(m_inst));
		m_buf = nullptr;
	}

	virtual bool create(const size_t len)
	{
		return false;
	}

	virtual bool create(uint8_t* const buf, const size_t len)
	{
		if(m_handle)
		{
			reset();
		}

		m_buf = buf;
		m_len = len;

		m_handle = xMessageBufferCreateStatic(m_len, m_buf, &m_inst);

		return m_handle != nullptr;
	}

	virtual void reset()
	{
		Message_buffer::reset();

		memset(&m_inst, 0, sizeof(m_inst));
		m_buf = nullptr;
	}

protected:
	StaticMessageBuffer_t m_inst;

	uint8_t* m_buf;
};
