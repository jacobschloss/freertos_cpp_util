/**
 * @brief Stack allocated mutex
 * @author Jacob Schloss <jacob@schloss.io>
 * @copyright Copyright (c) 2018 Jacob Schloss. All rights reserved.
 * @license Licensed under the 3-Clause BSD license. See License for details
*/

#pragma once

#include "freertos_cpp_util/Mutex_base.hpp"

class Mutex_static : public Mutex_base
{
public:
	Mutex_static()
	{
		m_mutex = xSemaphoreCreateMutexStatic(&m_mutex_buf);
	}
	~Mutex_static() override
	{

	}
protected:
	StaticSemaphore_t m_mutex_buf;
};
