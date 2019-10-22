/**
 * @brief Heap allocated mutex
 * @author Jacob Schloss <jacob@schloss.io>
 * @copyright Copyright (c) 2018 Jacob Schloss. All rights reserved.
 * @license Licensed under the 3-Clause BSD license. See LICENSE for details
*/

#pragma once

#include "freertos_cpp_util/Mutex_base.hpp"

class Mutex_heap : public Mutex_base
{
public:
	Mutex_heap()
	{
		m_mutex = xSemaphoreCreateMutex();
	}
	~Mutex_heap() override
	{
		vSemaphoreDelete(m_mutex);
	}
protected:

};
