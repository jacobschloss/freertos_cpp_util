/**
 * @brief Condition_variable
 * @author Jacob Schloss <jacob@schloss.io>
 * @copyright Copyright (c) 2018 Jacob Schloss. All rights reserved.
 * @license Licensed under the 3-Clause BSD license. See License for details
*/

#pragma once

#include "freertos_cpp_util/Mutex_static.hpp"
#include "freertos_cpp_util/CSema_static.hpp"
#include "freertos_cpp_util/BSema_static.hpp"
//#include "freertos_cpp_util/Critical_section.hpp"
#include "freertos_cpp_util/Queue_static_pod.hpp"
#include "freertos_cpp_util/Suspend_task_scheduler.hpp"

#include "freertos_cpp_util/Intrusive_list.hpp"

#include <atomic>
#include <mutex>

//similar to
//https://www.microsoft.com/en-us/research/wp-content/uploads/2004/12/ImplementingCVs.pdf

//implement a Condition variable with only a lifo and a bsemphr

class Condition_variable : private Non_copyable
{
public:

	Condition_variable()
	{
		m_task_queue_sema.give();
	}

	void notify_one()
	{
		{
			//lets wake this before letting others run
			Suspend_task_scheduler sched_suspend;

			m_task_queue_sema.take();
			if(!m_task_queue.empty())
			{
				//we need to ensure the lifetime of Waiter_node is long enough to pop it...
				//right now it is on the stack of the sleeping thread
				//this is currently handeled by locking the scheduler while we wake the sleeping tasks
				//maybe just move this to the heap

				m_task_queue.front<Waiter_node>()->m_bsema.give();
				m_task_queue.pop_front();
			}
			m_task_queue_sema.give();
		}

		//we might be pre-empted now, if preemption is turned on
	}

	void notify_all()
	{
		{
			//lets wake all of these before letting them run
			Suspend_task_scheduler sched_suspend;

			m_task_queue_sema.take();
			while(!m_task_queue.empty())
			{
				//we need to ensure the lifetime of Waiter_node is long enough to pop it...
				//right now it is on the stack of the sleeping thread
				//this is currently handeled by locking the scheduler while we wake the sleeping tasks
				//maybe just move this to the heap

				m_task_queue.front<Waiter_node>()->m_bsema.give();
				m_task_queue.pop_front();
			}
			m_task_queue_sema.give();
		}

		//we might be pre-empted now, if preemption is turned on
	}

	template< class Mutex >
	void wait(std::unique_lock<Mutex>& lock)
	{
		TaskHandle_t curr_task_handle = xTaskGetCurrentTaskHandle();

		//add us to a lifo queue
		Waiter_node node;
		m_task_queue_sema.take();
		m_task_queue.push_front(&node);
		m_task_queue_sema.give();

		//unlock lock, blocks the thread
		lock.unlock();
		node.m_bsema.take();

		//we are awake again
		lock.lock();
	}

	template< class Mutex, class Predicate >
	void wait(std::unique_lock<Mutex>& lock, Predicate pred)
	{
		while(!pred())
		{
			wait(lock);
		}
	}

protected:

	class Waiter_node : public Intrusive_list_fwd_node
	{
	public:
		Waiter_node()
		{

		}

		BSema_static m_bsema;
	};
	
	Intrusive_list_fwd m_task_queue;
	BSema_static m_task_queue_sema;
};
