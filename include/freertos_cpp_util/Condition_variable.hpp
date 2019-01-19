/**
 * @brief Condition_variable
 * @author Jacob Schloss <jacob@schloss.io>
 * @copyright Copyright (c) 2018 Jacob Schloss. All rights reserved.
 * @license Licensed under the 3-Clause BSD license. See LICENSE for details
*/

#pragma once

#include "freertos_cpp_util/BSema_static.hpp"
#include "freertos_cpp_util/Mutex_static.hpp"
#include "freertos_cpp_util/Suspend_task_scheduler.hpp"

#include "freertos_cpp_util/util/Intrusive_slist.hpp"
#include "freertos_cpp_util/util/Non_copyable.hpp"

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

	///
	/// Wake one waiting task
	/// You do not need to own the associated mutex, and in general holding the lock is bad for performance
	///
	/// This currently can cause a priority inversion, as the LIFO task queue will not necessarily wake the most important waiter
	///
	void notify_one()
	{
		while(true)
		{
			//lets wake this before letting others run
			//Currently required to enforce lifetime requirement of Waiter_node
			Suspend_task_scheduler sched_suspend;

			//we can't block while the scheduler is off
			if(!m_task_queue_sema.try_take())
			{
				continue;
			}

			if(!m_task_queue.empty())
			{
				//we need to ensure the lifetime of Waiter_node is long enough to pop it...
				//right now it is on the stack of the sleeping thread
				//this is currently handled by locking the scheduler while we wake the sleeping tasks
				//maybe just move this to the heap

				m_task_queue.front<Waiter_node>()->m_bsema.give();
				m_task_queue.pop_front();
			}
			m_task_queue_sema.give();
			break;
		}

		//we might be preempted now, if preemption is turned on
	}

	///
	/// Wake all waiting tasks
	/// You do not need to own the associated mutex, and in general holding the lock is bad for performance
	///
	void notify_all()
	{
		while(true)
		{
			//lets wake all of these before letting them run
			//Currently required to enforce lifetime requirement of Waiter_node
			//this also fixes the priority inversion caused by the lifo, since all the tasks will be runnable once this is over
			//although priority will be briefly inverted while we mark them all runnable
			Suspend_task_scheduler sched_suspend;

			//we can't block while the scheduler is off
			if(!m_task_queue_sema.try_take())
			{
				continue;
			}

			while(!m_task_queue.empty())
			{
				//we need to ensure the lifetime of Waiter_node is long enough to pop it...
				//right now it is on the stack of the sleeping thread
				//this is currently handled by locking the scheduler while we wake the sleeping tasks
				//maybe just move this to the heap

				m_task_queue.front<Waiter_node>()->m_bsema.give();
				m_task_queue.pop_front();
			}
			m_task_queue_sema.give();
			break;
		}

		//we might be preempted now, if preemption is turned on
	}

	///
	/// You must hold lock before calling this
	/// All waiters must lock the same mutex
	///
	template< class Mutex >
	void wait(std::unique_lock<Mutex>& lock)
	{
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

	///
	/// You must hold lock before calling this
	/// All waiters must lock the same mutex
	///
	template< class Mutex, class Predicate >
	void wait(std::unique_lock<Mutex>& lock, Predicate pred)
	{
		while(!pred())
		{
			wait(lock);
		}
	}

	///
	/// You must hold lock before calling this
	/// All waiters must lock the same mutex
	///
	template< class Mutex, class Rep, class Period, class Predicate >
	bool wait_for(std::unique_lock<Mutex>& lock, 
		const std::chrono::duration<Rep, Period>& rel_time,
		Predicate pred)
	{
		TimeOut_t timeout;
		vTaskSetTimeOutState(&timeout);
		TickType_t timeout_ticks_left = std::chrono::duration_cast<std::chrono::milliseconds>(rel_time).count();

		while(!pred())
		{
			//add us to a lifo queue
			Waiter_node node;
			m_task_queue_sema.take();
			m_task_queue.push_front(&node);
			m_task_queue_sema.give();

			//update waiting time left
			xTaskCheckForTimeOut(&timeout, &timeout_ticks_left);

			//unlock lock, blocks the thread
			lock.unlock();
			const bool got_sema = node.m_bsema.try_take_for_ticks(timeout_ticks_left);

			//false if we didn't get notified, and instead timed out
			if(!got_sema)
			{
				//we timed out, lock the list and remove us if we are still there
				m_task_queue_sema.take();
				m_task_queue.erase(&node);
				m_task_queue_sema.give();

				//one last try, maybe we got lucky and the predicate is true now
				lock.lock();
				return pred();
			}

			//we are awake again
			lock.lock();
		}

		return true;
	}

protected:

	class Waiter_node : public Intrusive_slist_node
	{
	public:
		Waiter_node() = default;
		~Waiter_node() = default;


		BSema_static m_bsema;
	};
	
	//A LIFO queue of waiting tasks
	//The node is allocated on the waiter's stack
	Intrusive_slist m_task_queue;
	///A binary semaphore that acts a mutex for m_task_queue
	BSema_static m_task_queue_sema;
};
