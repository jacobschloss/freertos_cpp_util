/**
 * @brief Stack allocated mutex
 * @author Jacob Schloss <jacob@schloss.io>
 * @copyright Copyright (c) 2025 Jacob Schloss. All rights reserved.
 * @license Licensed under the 3-Clause BSD license. See LICENSE for details
*/

#pragma once

#include "freertos_cpp_util/Event_group.hpp"

class Event_group_static : public Event_group
{
public:
	Event_group_static()
	{
		m_handle = xEventGroupCreateStatic(&m_inst);
	}

protected:
	StaticEventGroup_t m_inst;
};