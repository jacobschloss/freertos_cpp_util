/**
 * @brief Top util for FreeRTOS
 * @author Jacob Schloss <jacob@schloss.io>
 * @copyright Copyright (c) 2023 Jacob Schloss. All rights reserved.
 * @license Licensed under the 3-Clause BSD license. See LICENSE for details
*/

#include "freertos_cpp_util/Task_watcher.hpp"

void Task_watcher::work()
{
	const size_t dt_ms       = 5000;
	TickType_t xLastWakeTime = xTaskGetTickCount();
	TickType_t xPeriod       = pdMS_TO_TICKS(dt_ms);

	for(;;)
	{
		UBaseType_t num_set_t0 = getTaskStats(&taskStatus_t0.taskStatus, &taskStatus_t0.runTimeSinceBoot);
		if( num_set_t0 == 0 )
		{
			continue;
		}

		vTaskDelayUntil(&xLastWakeTime, xPeriod);

		UBaseType_t num_set_t1 = getTaskStats(&taskStatus_t1.taskStatus, &taskStatus_t1.runTimeSinceBoot);
		if( num_set_t1 == 0 )
		{
			continue;
		}

		if(calcStats(num_set_t0, taskStatus_t0, num_set_t1, taskStatus_t1))
		{

		}
	}
}

UBaseType_t Task_watcher::getTaskStats(TaskStatusArray* const pxTaskStatusArray, unsigned long * const pulTotalRunTime)
{
	UBaseType_t num_tasks = uxTaskGetNumberOfTasks();
	UBaseType_t num_set = uxTaskGetSystemState(pxTaskStatusArray->data(), pxTaskStatusArray->size(), pulTotalRunTime);

	if(num_set != num_tasks)
	{
		return 0;
	}

	return num_set;
}

bool Task_watcher::calcStats(size_t num_task_0, const TaskStats& t0, size_t num_task_1, const TaskStats& t1)
{
	Stack_string<512> out_msg;

	out_msg.sprintf("Name\tNum\tState\tHWM\tRT\r\n");

	const float dt_ms = float(t1.runTimeSinceBoot - t0.runTimeSinceBoot) / 1000.0f;

	for(size_t i = 0; i < num_task_0; i++)
	{
		TaskStatus_t const * const task_t0 = &(t0.taskStatus[i]);
		TaskStatus_t const * const task_t1 = get_task_by_number(task_t0->xTaskNumber, num_task_1, t1.taskStatus);
		if(task_t0 && task_t1)
		{
			const unsigned long dt_runtime = task_t1->ulRunTimeCounter - task_t0->ulRunTimeCounter;
			const float task_percent_cpu = 100.0f * (float(dt_runtime) / 1000.0f) / dt_ms;

			out_msg.sprintf("%s\t%u\t%s\t%u\t%.2f\r\n",
				task_t1->pcTaskName,
				unsigned(task_t1->xTaskNumber),
				task_state_to_str(task_t1->eCurrentState),
				unsigned(task_t1->usStackHighWaterMark),
				task_percent_cpu
			);
		}
	}

	{
		HeapStats_t stat;
		vPortGetHeapStats(&stat);

		out_msg.sprintf("Heap Stats\r\n\tSize: %u\r\n\tAvail: %u\r\n\tMinAvail: %u\r\n",
			unsigned(configTOTAL_HEAP_SIZE),
			unsigned(stat.xAvailableHeapSpaceInBytes),
			unsigned(stat.xMinimumEverFreeBytesRemaining)
		);
	}
	
	if(out_msg.full())
	{
		m_print_handler(out_msg.data());
		out_msg.clear();
		out_msg.sprintf("Error - task stats overrun");
		m_print_handler(out_msg.data());
	}
	else
	{
		m_print_handler(out_msg.data());
	}


	return false;
}

TaskStatus_t const * Task_watcher::get_task_by_number(UBaseType_t task_number, size_t num_task, const TaskStatusArray& tasks)
{
	for(size_t i = 0; i < num_task; i++)
	{
		if(task_number == tasks[i].xTaskNumber)
		{
			return &(tasks[i]);
		}
	}

	return nullptr;
}

const char* Task_watcher::task_state_to_str(const eTaskState state)
{
	char const * msg = "UNK";
	switch(state)
	{
		case eReady:
		{
			msg = "RDY";
			break;
		}
		case eRunning:
		{
			msg = "RUN";
			break;
		}
		case eBlocked:
		{
			msg = "BLK";
			break;
		}
		case eSuspended:
		{
			msg = "SUS";
			break;
		}
		case eDeleted:
		{
			msg = "DEL";
			break;
		}
		default:
		{
			msg = "UNK";
			break;
		}
	}
	return msg;
}
