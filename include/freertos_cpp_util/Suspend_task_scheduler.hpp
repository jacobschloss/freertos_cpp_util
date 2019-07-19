/**
 * @brief RAII task scheduler lock
 * @author Jacob Schloss <jacob@schloss.io>
 * @copyright Copyright (c) 2018 Jacob Schloss. All rights reserved.
 * @license Licensed under the 3-Clause BSD license. See LICENSE for details
*/

#pragma once

#include "common_util/Non_copyable.hpp"

#include "FreeRTOS.h"
#include "task.h"

class Suspend_task_scheduler : private Non_copyable
{
public:
	Suspend_task_scheduler()
	{
		vTaskSuspendAll();
	}

	~Suspend_task_scheduler()
	{
		BaseType_t ret = xTaskResumeAll();

		if(ret == pdFALSE )
		{
			taskYIELD();
		}
	}
};