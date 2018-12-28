/**
 * @brief Static binary semaphore
 * @author Jacob Schloss <jacob@schloss.io>
 * @copyright Copyright (c) 2018 Jacob Schloss. All rights reserved.
 * @license Licensed under the 3-Clause BSD license. See LICENSE for details
*/

#pragma once

#include "freertos_cpp_util/Semaphore_base.hpp"

class BSema_static : public Semaphore_base
{
public:

	BSema_static()
	{
		m_sema = xSemaphoreCreateBinaryStatic(&m_sema_buf);
	}

	~BSema_static() override
	{
		
	}

protected:

	StaticSemaphore_t m_sema_buf;

};