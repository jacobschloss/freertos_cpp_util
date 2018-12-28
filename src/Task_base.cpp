/**
 * @author Jacob Schloss <jacob@schloss.io>
 * @copyright Copyright (c) 2018 Jacob Schloss. All rights reserved.
 * @license Licensed under the 3-Clause BSD license. See LICENSE for details
*/

#include "freertos_cpp_util/Task_base.hpp"

extern "C"
{
	void FreeRTOS_task_dispatch(void* ctx)
	{
		Task_base* inst = static_cast<Task_base*>(ctx);

		inst->work();

		//in case work returns, remove us from the runnable list
		//we could also self-delete, but on some combination of port and newlib that crashes
		for(;;)
		{
			vTaskSuspend(nullptr);
		}
	}
}

Task_base::Task_base()
{
	m_handle = nullptr;
}

Task_base::~Task_base()
{
	if(m_handle)
	{
		vTaskDelete(m_handle);
		m_handle = nullptr;
	}
}

void Task_base::work()
{
	for(;;)
	{
		vTaskSuspend(nullptr);
	}
}

char* Task_base::get_name()
{
	return pcTaskGetName(m_handle);
}
TaskHandle_t Task_base::get_handle() const
{
	return m_handle;
}
eTaskState Task_base::get_state()
{
	return eTaskGetState(m_handle);
}
UBaseType_t Task_base::get_priority()
{
	return uxTaskPriorityGet(m_handle);
}

void Task_base::suspend()
{
	vTaskSuspend(m_handle);
}
void Task_base::resume()
{
	vTaskResume(m_handle);
}

void Task_base::resume_isr()
{
	const BaseType_t xYieldRequired = xTaskResumeFromISR(m_handle);

	portYIELD_FROM_ISR(xYieldRequired);
}