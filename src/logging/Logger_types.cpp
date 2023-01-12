/**
 * @brief Log_sink_base
 * @author Jacob Schloss <jacob@schloss.io>
 * @copyright Copyright (c) 2019 Jacob Schloss. All rights reserved.
 * @license Licensed under the 3-Clause BSD license. See LICENSE for details
*/

#include "freertos_cpp_util/logging/Logger_types.hpp"

namespace freertos_util
{

namespace logging
{

	const char* LOG_LEVEL_to_str(const LOG_LEVEL lvl)
	{
		char const* str = nullptr;

		switch(lvl)
		{
			case LOG_LEVEL::fatal:
			{
				str = "fatal";
				break;
			}
			case LOG_LEVEL::error:
			{
				str = "error";
				break;
			}
			case LOG_LEVEL::warn:
			{
				str = "warn";
				break;
			}
			case LOG_LEVEL::info:
			{
				str = "info";
				break;
			}
			case LOG_LEVEL::debug:
			{
				str = "debug";
				break;
			}
			case LOG_LEVEL::trace:
			{
				str = "trace";
				break;
			}
			default:
			{
				str = "invalid";
				break;
			}
		}

		return str;
	}
}

}


