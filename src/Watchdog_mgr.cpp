#include "freertos_cpp_util/Watchdog_mgr.hpp"

#include <mutex>

Watchdog_mgr::Watchdog_mgr()
{
	m_wd_reset_func = nullptr;
	m_wd_reset_func_ctx = nullptr;

	m_task_wd_period = std::chrono::milliseconds(100);
}

bool Watchdog_mgr::initialize()
{
	return true;
}

void Watchdog_mgr::register_task(const TaskHandle_t task_handle, Watchdog_token_handle* const out_token)
{
	{
		std::unique_lock<Mutex_static> lock(m_mutex);

		*out_token = nullptr;
		for(size_t i = 0; i < m_watchdog_tokens.size(); i++)
		{
			if(m_watchdog_tokens[i].m_task_handle == nullptr)
			{
				*out_token = &m_watchdog_tokens[i];
			
				(*out_token)->m_task_handle = task_handle;
				(*out_token)->m_idx = i;

				(*out_token)->reset();

				break;
			}
		}
	}

	//check in the first time
	task_checkin(*out_token);
}

void Watchdog_mgr::unregister_task(const Watchdog_token_handle token)
{
	std::unique_lock<Mutex_static> lock(m_mutex);

	if(token->m_task_handle == nullptr)
	{
		return;
	}
	
	token->m_task_handle = nullptr;
}


void Watchdog_mgr::task_checkin(const Watchdog_token_handle token)
{
	token->reset();

	//really no point in notifying the other thread
	// m_activity_cond_var.notify_one();
}

void Watchdog_mgr::work()
{
	std::function<bool()> timeout_pred = std::bind(&Watchdog_mgr::check_for_timeout, this);

	for(;;)
	{
		bool task_timeout = false;

		{
			std::unique_lock<Mutex_static> lock(m_mutex);
		
			task_timeout = m_activity_cond_var.wait_for(lock, m_task_wd_period, timeout_pred);
		}

		if(!task_timeout)
		{
			if(m_wd_reset_func != nullptr)
			{
				m_wd_reset_func(m_wd_reset_func_ctx);			
			}
		}
		else
		{
			//We had a wd violation
			//turn off isr & scheduler
			//loop to cause a WD reset 
			Critical_section crit_sec;
			for(;;)
			{

			}
		}
	}
}