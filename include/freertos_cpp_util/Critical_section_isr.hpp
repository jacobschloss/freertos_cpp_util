/**
 * @brief RAII isr critical section
 * @author Jacob Schloss <jacob@schloss.io>
 * @copyright Copyright (c) 2018 Jacob Schloss. All rights reserved.
 * @license Licensed under the 3-Clause BSD license. See License for details
*/

#pragma once

#include "FreeRTOS.h"
#include "task.h"

class Critical_section_isr
{
public:
	Critical_section_isr()
	{
		m_mask = taskENTER_CRITICAL_FROM_ISR();
	}

	~Critical_section_isr()
	{
		taskEXIT_CRITICAL_FROM_ISR(m_mask);
	}
protected:
	UBaseType_t m_mask;
};