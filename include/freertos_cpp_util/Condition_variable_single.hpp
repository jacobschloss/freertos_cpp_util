/**
 * @brief Condition_variable
 * @author Jacob Schloss <jacob@schloss.io>
 * @copyright Copyright (c) 2018 Jacob Schloss. All rights reserved.
 * @license Licensed under the 3-Clause BSD license. See LICENSE for details
*/

#pragma once

#include "freertos_cpp_util/BSema_static.hpp"
#include "freertos_cpp_util/Suspend_task_scheduler.hpp"

#include "freertos_cpp_util/util/Intrusive_slist.hpp"
#include "freertos_cpp_util/util/Non_copyable.hpp"

#include <atomic>
#include <mutex>

//A binary semaphore with a condition variable like api
//Useful when a single thread waits for notification
//Allows a predicate to be waited on with timeout
//Unlike the full Condition_variable there is no waiter list
//and only one context may call any wait function

class Condition_variable_single : private Non_copyable
{
public:

	Condition_variable()
	{
		m_has_waiter.store(false);
	}

	///
	/// Wake one waiting task
	/// You do not need to own the associated mutex, and in general holding the lock is bad for performance
	///
	/// This currently can cause a priority inversion, as the LIFO task queue will not necessarily wake the most important waiter
	///
	void notify_one()
	{
		m_task_queue_sema.take();

		if(m_has_waiter.load())
		{
			m_has_waiter.store(false);
			m_sema.give();
		}

		m_task_queue_sema.give();
	}

	///
	/// You must hold lock before calling this
	/// All waiters must lock the same mutex
	///
	template< class Mutex >
	void wait(std::unique_lock<Mutex>& lock)
	{
		m_task_queue_sema.take();
		m_has_waiter.store(true);
		m_task_queue_sema.give();

		//unlock lock, blocks the thread
		lock.unlock();
		m_sema.take();

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

	enum class cv_status
	{
		no_timeout,
		timeout
	};

	///
	/// You must hold lock before calling this
	/// All waiters must lock the same mutex
	///
	template< class Mutex >
	cv_status wait_for_ticks(std::unique_lock<Mutex>& lock, const TickType_t ticks)
	{
		m_task_queue_sema.take();
		m_has_waiter.store(true);
		m_task_queue_sema.give();

		//unlock lock, blocks the thread
		lock.unlock();
		const bool got_sema = m_sema.try_take_for_ticks(ticks);

		//default no timeout
		cv_status ret = cv_status::no_timeout;

		//true if we were notified
		//false if we didn't get notified, and instead timed out
		if(!got_sema)
		{
			//we timed out
			ret = cv_status::timeout;
		}

		//we are awake again, if no timeout the notifier removed us from the task queue list
		lock.lock();

		return ret;
	}

	///
	/// You must hold lock before calling this
	/// All waiters must lock the same mutex
	///
	template< class Mutex, class Rep, class Period >
	cv_status wait_for(std::unique_lock<Mutex>& lock, 
		const std::chrono::duration<Rep, Period>& rel_time)
	{
		const TickType_t timeout_ticks = std::chrono::duration_cast<std::chrono::milliseconds>(rel_time).count();

		return wait_for_ticks(lock, timeout_ticks);
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
			//update waiting time left
			xTaskCheckForTimeOut(&timeout, &timeout_ticks_left);

			//wait for timeout or notification
			const cv_status status = wait_for_ticks(lock, timeout_ticks_left);

			//if we timed out, don't keep spinning
			if(status == cv_status::timeout)
			{
				//one last try, maybe we got lucky and the predicate is true now
				return pred();
			}

			//otherwise no timeout, so spin around and check pred() and sleep again
		}

		return true;
	}

protected:

	std::atomic<bool> m_has_waiter;

	///A binary semaphore that acts a mutex for m_task_queue
	BSema_static m_task_queue_sema;

	///A binary semaphore that acts a mutex for m_task_queue
	BSema_static m_sema;
};
