/**
 * @brief RAII critial section
 * @author Jacob Schloss <jacob@schloss.io>
 * @copyright Copyright (c) 2018 Jacob Schloss. All rights reserved.
 * @license Licensed under the 3-Clause BSD license. See License for details
*/

#pragma once

#include "FreeRTOS.h"
#include "task.h"

class Critical_section
{
public:
	Critical_section()
	{
		taskENTER_CRITICAL();
	}

	~Critical_section()
	{
		taskEXIT_CRITICAL();
	}
};