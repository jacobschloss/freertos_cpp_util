/**
 * @brief Base task class
 * @author Jacob Schloss <jacob@schloss.io>
 * @copyright Copyright (c) 2018 Jacob Schloss. All rights reserved.
 * @license Licensed under the 3-Clause BSD license. See License for details
*/

#pragma once

#include "FreeRTOS.h"
#include "task.h"

extern "C"
{
	void FreeRTOS_task_dispatch(void* ctx);
}

class Task_base
{
public:

	Task_base();
	virtual ~Task_base();

	virtual void work();

protected:
	TaskHandle_t m_handle;
};