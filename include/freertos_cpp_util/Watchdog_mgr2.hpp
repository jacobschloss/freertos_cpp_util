#pragma once

//this shares a single timeout for all tasks

//a time wheel might be better, then we properly track per-task timeout

#include "freertos_cpp_util/Condition_variable.hpp"
#include "freertos_cpp_util/Task_base.hpp"
#include "freertos_cpp_util/Mutex_static.hpp"
#include "freertos_cpp_util/Critical_section.hpp"

#include "freertos_cpp_util/util/Intrusive_list.hpp"

#include <atomic>
#include <array>

class Watchdog_mgr2;
class Watchdog_token2;

class Watchdog_mgr2 : Non_copyable
{
public:

	class Watchdog_token : public Intrusive_list_node
	{
	public:
		friend Watchdog_mgr2;

		Watchdog_token() : m_state(false)
		{

		}
		~Watchdog_token() = default;
	
	protected:

		void reset()
		{
			m_ticks_left = m_period;
			vTaskSetTimeOutState(&m_timeout);

			m_state.clear();
		}

		TickType_t ticks_left() const
		{
			return m_ticks_left;
		}

	private:
		TaskHandle_t m_task_handle;
		TickType_t m_period;
		
		TimeOut_t m_timeout;
		TickType_t m_ticks_left;

		//false = ok
		//true = timeout
		std::atomic_flag m_state;
	};

	typedef Watchdog_token* Watchdog_token_handle;

	typedef void(*WatchdogResetCallback)(void* ctx);

	Watchdog_mgr2();
	virtual ~Watchdog_mgr2();

	bool initialize();

	void register_task(const TaskHandle_t task_handle, Watchdog_token_handle* const out_token);
	void unregister_task(const Watchdog_token_handle token);

	void task_checkin(const Watchdog_token_handle out_token);

	void set_watchdog_reset_callback(const WatchdogResetCallback callback, void* ctx);

	void work();

protected:

	//true on timeout
	bool check_for_timeout()
	{
		bool task_timeout = false;

		Watchdog_token* node = m_task_list.front<Watchdog_token>();

		//update the timeouts
		do
		{
			if(node->m_state.test_and_set())
			{
				task_timeout = true;
				break;
			}

			node = static_cast<Watchdog_token*>(node->next());

		} while(node != nullptr);

		return task_timeout;		
	}
	
	Intrusive_list m_task_list;

	Mutex_static m_mutex;
	Condition_variable m_activity_cond_var;
};