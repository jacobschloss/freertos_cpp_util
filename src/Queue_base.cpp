/**
 * @author Jacob Schloss <jacob@schloss.io>
 * @copyright Copyright (c) 2018 Jacob Schloss. All rights reserved.
 * @license Licensed under the 3-Clause BSD license. See License for details
*/

#include "freertos_cpp_util/Queue_base.hpp"

Queue_base::Queue_base()
{
	m_queue = nullptr;
}
Queue_base::~Queue_base()
{
	if(m_queue)
	{
		//if we've been named, remove us
		const char* name = pcQueueGetName(m_queue);
		if(name)
		{
			vQueueUnregisterQueue(m_queue);
		}

		vQueueDelete(m_queue);
		m_queue = nullptr;
	}
}