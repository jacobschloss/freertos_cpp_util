/**
 * @brief Log_sink_console
 * @author Jacob Schloss <jacob@schloss.io>
 * @copyright Copyright (c) 2019 Jacob Schloss. All rights reserved.
 * @license Licensed under the 3-Clause BSD license. See LICENSE for details
*/

#pragma once

#include "freertos_cpp_util/logging/Log_sink_base.hpp"

namespace freertos_util
{
	namespace logging
	{
		class Log_sink_console : public Log_sink_base
		{
			public:

			~Log_sink_console() override
			{
				
			}

			bool handle_log(String_type* const log) override;

			protected:
		};
	}
}
