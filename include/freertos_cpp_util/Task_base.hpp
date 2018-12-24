/**
 * @brief Base task class
 * @author Jacob Schloss <jacob@schloss.io>
 * @copyright Copyright (c) 2018 Jacob Schloss. All rights reserved.
 * @license Licensed under the 3-Clause BSD license. See License for details
*/

#pragma once

#include "freertos_cpp_util/Non_copyable.hpp"

#include "FreeRTOS.h"
#include "task.h"

extern "C"
{
	void FreeRTOS_task_dispatch(void* ctx);
}

class Task_base : private Non_copyable
{
public:

	Task_base();
	virtual ~Task_base();

	virtual void work();

	char* get_name();
	TaskHandle_t get_handle() const;
	eTaskState get_state();
	UBaseType_t get_priority();

	void suspend();
	void resume();
	void resume_isr();

protected:
	TaskHandle_t m_handle;
};