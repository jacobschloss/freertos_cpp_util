/**
 * @brief Static binary semaphore
 * @author Jacob Schloss <jacob@schloss.io>
 * @copyright Copyright (c) 2018 Jacob Schloss. All rights reserved.
 * @license Licensed under the 3-Clause BSD license. See LICENSE for details
*/

#pragma once

#include "freertos_cpp_util/Semaphore_base.hpp"

class CSema_static : public Semaphore_base
{
public:

	CSema_static(const UBaseType_t max_count, const UBaseType_t start_count)
	{
		m_sema = xSemaphoreCreateCountingStatic(max_count, start_count, &m_sema_buf);
	}

	~CSema_static() override
	{
		
	}

protected:

	StaticSemaphore_t m_sema_buf;

};