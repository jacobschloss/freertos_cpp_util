#include "freertos_cpp_util/Watchdog_mgr2.hpp"

bool Watchdog_mgr2::initialize()
{
	return true;
}

void Watchdog_mgr2::register_task(const TaskHandle_t task_handle, Watchdog_token_handle* const out_token)
{
	Watchdog_token_handle node = *out_token;

	node->m_task_handle = task_handle;
	node->reset();
	
	{
		std::unique_lock<Mutex_static> lock(m_mutex);
		
		m_task_list.push_back(node);
		
		//update current timeout
		m_activity_cond_var.notify_one();
	}
}

void Watchdog_mgr2::work()
{
	for(;;)
	{
		{
			std::unique_lock<Mutex_static> lock(m_mutex);

			const Condition_variable::cv_status timeout = m_activity_cond_var.wait_for_ticks(lock, m_task_list.front<Watchdog_token>()->ticks_left());

			switch(timeout)
			{
				case Condition_variable::cv_status::no_timeout:
				{
					//someone added a node
					Watchdog_token* node = m_task_list.front<Watchdog_token>();

					//update the timeouts
					do
					{
						BaseType_t ret = xTaskCheckForTimeOut(&node->m_timeout, &node->m_ticks_left);

						if(ret == pdTRUE)
						{
							//we timed out that is bad
							Critical_section crit_sec;
							for(;;)
							{

							}
						}

						node = static_cast<Watchdog_token*>(node->next());

					} while(node != nullptr);

					//sort m_task_list

					break;
				}
				case Condition_variable::cv_status::timeout:
				{
					//we timed out that is bad
					Critical_section crit_sec;
					for(;;)
					{

					}

					break;
				}
				default:
				{
					//invalid state
					Critical_section crit_sec;
					for(;;)
					{
						
					}
					break;
				}
			}
		}
	}
}
