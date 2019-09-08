/**
 * @brief Log_sink_base
 * @author Jacob Schloss <jacob@schloss.io>
 * @copyright Copyright (c) 2019 Jacob Schloss. All rights reserved.
 * @license Licensed under the 3-Clause BSD license. See LICENSE for details
*/

#pragma once

#include "freertos_cpp_util/logging/Logger_types.hpp"

namespace freertos_util
{
	namespace logging
	{
		class Log_sink_base
		{
			public:

			virtual ~Log_sink_base()
			{
				
			}

			virtual bool handle_log(String_type* const log) = 0;

			protected:
		};
	}
}
