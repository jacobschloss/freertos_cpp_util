/**
 * @brief Base queue class
 * @author Jacob Schloss <jacob@schloss.io>
 * @copyright Copyright (c) 2018 Jacob Schloss. All rights reserved.
 * @license Licensed under the 3-Clause BSD license. See License for details
*/

#pragma once

#include "freertos_cpp_util/Non_copyable.hpp"

#include "FreeRTOS.h"
#include "queue.h"

#include <chrono>
#include <type_traits>

class Queue_base : private Non_copyable
{
public:

	Queue_base();
	virtual ~Queue_base();

	void clear()
	{
		xQueueReset(m_queue);
	}

	size_t size() const
	{
		return uxQueueMessagesWaiting(m_queue);
	}

	size_t size_isr() const
	{
		return uxQueueMessagesWaitingFromISR(m_queue);
	}

	bool full() const
	{
		return 0 == uxQueueSpacesAvailable(m_queue);
	}

	bool full_isr() const
	{
		BaseType_t ret = xQueueIsQueueFullFromISR(m_queue);

		return pdFALSE != ret;
	}

	bool empty() const
	{
		return 0 == size();
	}

	bool empty_isr() const
	{
		BaseType_t ret = xQueueIsQueueEmptyFromISR(m_queue);

		return pdFALSE != ret;
	}

	void set_name(const char* name)
	{
		vQueueAddToRegistry(m_queue, name);
	}

protected:

	QueueHandle_t m_queue;
};

template <typename T>
class Queue_template_base : public Queue_base
{
public:

	virtual bool pop_front(T* const item) = 0;

	virtual bool pop_front(T* const item, const TickType_t xTicksToWait) = 0;

	template< class Rep, class Period >
	bool pop_front(T* const item, const std::chrono::duration<Rep,Period>& duration)
	{
		std::chrono::milliseconds duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
		return pop_front(item, pdMS_TO_TICKS(duration_ms.count()));
	}

	virtual bool pop_front_isr(T* const item) = 0;

	virtual bool pop_front_isr(T* const item, BaseType_t* const pxHigherPriorityTaskWoken) = 0;

	virtual bool push_back(const T& item) = 0;

	virtual bool push_back(const T& item, const TickType_t xTicksToWait) = 0;

	template< class Rep, class Period >
	bool push_back(const T& item, const std::chrono::duration<Rep,Period>& duration)
	{
		std::chrono::milliseconds duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
		return push_back(item, pdMS_TO_TICKS(duration_ms.count()));
	}

	virtual bool push_front(const T& item) = 0;

	virtual bool push_front(const T& item, const TickType_t xTicksToWait) = 0;

	template< class Rep, class Period >
	bool push_front(const T& item, const std::chrono::duration<Rep,Period>& duration)
	{
		std::chrono::milliseconds duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
		return push_front(item, pdMS_TO_TICKS(duration_ms.count()));
	}

	virtual bool push_front_isr(const T& item) = 0;

	virtual bool push_front_isr(const T& item, BaseType_t* const pxHigherPriorityTaskWoken) = 0;

	virtual bool push_back_isr(const T& item) = 0;

	virtual bool push_back_isr(const T& item, BaseType_t* const pxHigherPriorityTaskWoken) = 0;
};

template <typename T>
class Queue_template_base_pod : public Queue_template_base<T>
{
public:

	static_assert(std::is_pod<T>::value, "T must be POD");

	bool pop_front(T* const item) override
	{
		return pop_front(item, 0);
	}

	bool pop_front(T* const item, const TickType_t xTicksToWait) override
	{
		return xQueueReceive(this->m_queue, item, xTicksToWait) == pdTRUE;
	}

	bool pop_front_isr(T* const item) override
	{
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		
		const bool ret = pop_front_isr(item, &xHigherPriorityTaskWoken);

		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

		return ret;
	}

	bool pop_front_isr(T* const item, BaseType_t* const pxHigherPriorityTaskWoken) override
	{
		return pdTRUE == xQueueReceiveFromISR(this->m_queue, item, pxHigherPriorityTaskWoken);
	}

	bool push_back(const T& item) override
	{
		return push_back(item, 0);
	}

	bool push_back(const T& item, const TickType_t xTicksToWait) override
	{
		return xQueueSendToBack(this->m_queue, &item, xTicksToWait) == pdTRUE;
	}

	bool push_front(const T& item) override
	{
		return push_front(item, 0);
	}

	bool push_front(const T& item, const TickType_t xTicksToWait) override
	{
		return xQueueSendToFront(this->m_queue, &item, xTicksToWait) == pdTRUE;
	}

	bool push_front_isr(const T& item) override
	{
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		
		const bool ret = push_front_isr(item, &xHigherPriorityTaskWoken);

		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

		return ret;
	}

	bool push_front_isr(const T& item, BaseType_t* const pxHigherPriorityTaskWoken) override
	{
		return pdTRUE == xQueueSendToFrontFromISR(this->m_queue, &item, pxHigherPriorityTaskWoken);
	}

	bool push_back_isr(const T& item) override
	{
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		
		const bool ret = push_back_isr(item, &xHigherPriorityTaskWoken);

		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

		return ret;
	}

	bool push_back_isr(const T& item, BaseType_t* const pxHigherPriorityTaskWoken) override
	{
		return pdTRUE == xQueueSendToBackFromISR(this->m_queue, &item, pxHigherPriorityTaskWoken);
	}
};
