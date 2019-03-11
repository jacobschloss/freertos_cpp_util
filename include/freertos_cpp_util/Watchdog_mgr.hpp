#pragma once

//this shares a single timeout for all tasks

//a time wheel might be better, then we properly track per-task timeout

#include "freertos_cpp_util/Condition_variable.hpp"
#include "freertos_cpp_util/Task_base.hpp"
#include "freertos_cpp_util/Mutex_static.hpp"
#include "freertos_cpp_util/Critical_section.hpp"

#include <atomic>
#include <array>

class Watchdog_mgr;
class Watchdog_token;

class Watchdog_mgr : Non_copyable
{
public:

	class Watchdog_token
	{
	public:
		friend Watchdog_mgr;

		Watchdog_token() : m_state(false)
		{
			m_task_handle = nullptr;
			m_idx = 0;
		}
		~Watchdog_token() = default;
	
	protected:

		bool test_and_set()
		{
			return m_state.test_and_set();	
		}

		void reset()
		{
			m_state.clear();
		}

	private:
		std::atomic<TaskHandle_t> m_task_handle;
		std::atomic<size_t> m_idx;

		std::atomic_flag m_state;
	};

	typedef Watchdog_token* Watchdog_token_handle;

	typedef void(*WatchdogResetCallback)(void* ctx);

	Watchdog_mgr();
	virtual ~Watchdog_mgr();

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

		for(size_t i = 0; i < m_watchdog_tokens.size(); i++)
		{
			Watchdog_token& node = m_watchdog_tokens[i];
			if(node.m_task_handle != nullptr)
			{
				if(node.test_and_set())
				{
					task_timeout = true;
					break;
				}
			}
		}

		return task_timeout;		
	}
	
	#if configUSE_16_BIT_TICKS == 0
		constexpr static size_t MAX_WD_TAKS = 24;
	#elif configUSE_16_BIT_TICKS == 1
		constexpr static size_t MAX_WD_TAKS = 8;
	#else
		constexpr static size_t MAX_WD_TAKS = 0;
	#endif

	Mutex_static m_mutex;
	Condition_variable m_activity_cond_var;

	std::chrono::milliseconds m_task_wd_period;

	WatchdogResetCallback m_wd_reset_func;
	void* m_wd_reset_func_ctx;

	std::array<Watchdog_token, MAX_WD_TAKS> m_watchdog_tokens;
};