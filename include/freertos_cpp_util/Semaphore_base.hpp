/**
 * @brief Base semaphore class
 * @author Jacob Schloss <jacob@schloss.io>
 * @copyright Copyright (c) 2018 Jacob Schloss. All rights reserved.
 * @license Licensed under the 3-Clause BSD license. See License for details
*/

#pragma once

#include "FreeRTOS.h"
#include "semphr.h"

class Semaphore_base
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
		return pdTRUE == xSemaphoreTake(m_sema, portMAX_DELAY);
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

	UBaseType_t get_count() const
	{
		return uxSemaphoreGetCount(m_sema);
	}

protected:

	SemaphoreHandle_t m_sema;
};