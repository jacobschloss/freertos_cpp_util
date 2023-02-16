/**
 * @brief Top util for FreeRTOS
 * @author Jacob Schloss <jacob@schloss.io>
 * @copyright Copyright (c) 2023 Jacob Schloss. All rights reserved.
 * @license Licensed under the 3-Clause BSD license. See LICENSE for details
*/

#include "freertos_cpp_util/Task_static.hpp"
#include "freertos_cpp_util/Task_heap.hpp"
#include "common_util/Stack_string.hpp"

#include <array>
#include <functional>

class Task_watcher : public Task_heap_tmpl<1024>
{
public:
	typedef std::array<TaskStatus_t, 16> TaskStatusArray;

	struct TaskStats
	{
		TaskStatusArray taskStatus;
		unsigned long   runTimeSinceBoot;
	};

	void work();

	static UBaseType_t getTaskStats(TaskStatusArray* const pxTaskStatusArray, unsigned long * const pulTotalRunTime);
	static TaskStatus_t const * get_task_by_number(UBaseType_t task_number, size_t num_task, const TaskStatusArray& tasks);
	static const char* task_state_to_str(const eTaskState state);

	bool calcStats(size_t num_task_0, const TaskStats& t0, size_t num_task_1, const TaskStats& t1);

	void set_print_handler(const std::function<void(const char*)>& handler)
	{
		m_print_handler = handler;
	}

protected:

	std::function<void(const char*)> m_print_handler;

	TaskStats taskStatus_t0;
	TaskStats taskStatus_t1;
};

