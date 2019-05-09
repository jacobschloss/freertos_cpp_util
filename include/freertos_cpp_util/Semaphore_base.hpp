/**
 * @brief Base semaphore class
 * @author Jacob Schloss <jacob@schloss.io>
 * @copyright Copyright (c) 2018 Jacob Schloss. All rights reserved.
 * @license Licensed under the 3-Clause BSD license. See LICENSE for details
*/

#pragma once

#include "freertos_cpp_util/util/Non_copyable.hpp"

#include "FreeRTOS.h"
#include "semphr.h"

#include <chrono>

class Semaphore_base : private Non_copyable
{
public:

	Semaphore_base()
	{
		m_sema = nullptr;
	}

	virtual ~Semaphore_base()
	{
		vSemaphoreDelete(m_sema);
		m_sema = nullptr;
	}

	bool take()
	{
		while(!try_take_for_ticks(portMAX_DELAY))
		{

		}

		return true;
	}

	bool try_take()
	{
		return try_take_for_ticks(0);
	}

	template< class Rep, class Period >
	bool try_take_for(const std::chrono::duration<Rep,Period>& duration)
	{
		std::chrono::milliseconds duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
		return try_take_for_ticks( pdMS_TO_TICKS(duration_ms.count()) );
	}

	bool try_take_for_ticks(const TickType_t ticks)
	{
		return pdTRUE == xSemaphoreTake(m_sema, ticks);
	}

	bool take_from_isr()
	{
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		const BaseType_t ret = xSemaphoreTakeFromISR(m_sema, &xHigherPriorityTaskWoken);
		
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

		return pdTRUE == ret;
	}

	bool give()
	{
		return pdTRUE == xSemaphoreGive(m_sema);
	}

	bool give_from_isr()
	{
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		
		const BaseType_t ret = xSemaphoreGiveFromISR(m_sema, &xHigherPriorityTaskWoken);

		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

		return pdTRUE == ret;
	}

	bool give_from_isr(BaseType_t* const pxHigherPriorityTaskWoken)
	{
		const BaseType_t ret = xSemaphoreGiveFromISR(m_sema, pxHigherPriorityTaskWoken);

		return pdTRUE == ret;
	}

	UBaseType_t get_count() const
	{
		return uxSemaphoreGetCount(m_sema);
	}

protected:

	SemaphoreHandle_t m_sema;
};