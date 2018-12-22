/**
 * @brief Static binary semaphore
 * @author Jacob Schloss <jacob@schloss.io>
 * @copyright Copyright (c) 2018 Jacob Schloss. All rights reserved.
 * @license Licensed under the 3-Clause BSD license. See License for details
*/

#pragma once

#include "freertos_cpp_util/Semaphore_base.hpp"

class BSema_static : public Semaphore_base
{
public:

	BSema_static()
	{
		m_sema = xSemaphoreCreateBinaryStatic(&m_sema_buf);
	}

	~BSema_static() override
	{
		
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

protected:

	StaticSemaphore_t m_sema_buf;

};