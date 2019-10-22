/**
 * @brief Stack allocated recursvoe mutex
 * @author Jacob Schloss <jacob@schloss.io>
 * @copyright Copyright (c) 2019 Jacob Schloss. All rights reserved.
 * @license Licensed under the 3-Clause BSD license. See LICENSE for details
*/

#pragma once

#include "freertos_cpp_util/Mutex_base.hpp"

class Mutex_static_recursive : public Mutex_base
{
public:
	Mutex_static_recursive()
	{
		m_mutex = xSemaphoreCreateRecursiveMutexStatic(&m_mutex_buf);
	}
	~Mutex_static_recursive() override
	{

	}

	bool try_lock_for_ticks(const TickType_t ticks) override
	{
		return pdTRUE == xSemaphoreTakeRecursive(m_mutex, ticks);
	}

	void unlock() override
	{
		xSemaphoreGiveRecursive(m_mutex);
	}

protected:
	StaticSemaphore_t m_mutex_buf;
};
