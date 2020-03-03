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
			case LOG_LEVEL::FATAL:
			{
				str = "FATAL";
				break;
			}
			case LOG_LEVEL::ERROR:
			{
				str = "ERROR";
				break;
			}
			case LOG_LEVEL::WARN:
			{
				str = "WARN";
				break;
			}
			case LOG_LEVEL::INFO:
			{
				str = "INFO";
				break;
			}
			case LOG_LEVEL::DEBUG:
			{
				str = "DEBUG";
				break;
			}
			case LOG_LEVEL::DEBUG_VERBOSE:
			{
				str = "DEBUG_VERBOSE";
				break;
			}
			case LOG_LEVEL::TRACE:
			{
				str = "TRACE";
				break;
			}
			case LOG_LEVEL::TRACE_VERBOSE:
			{
				str = "TRACE_VERBOSE";
				break;
			}
			default:
			{
				str = "Invalid";
				break;
			}
		}

		return str;
	}
}

}


