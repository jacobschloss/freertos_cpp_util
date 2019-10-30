/**
 * @brief Log_sink_console
 * @author Jacob Schloss <jacob@schloss.io>
 * @copyright Copyright (c) 2019 Jacob Schloss. All rights reserved.
 * @license Licensed under the 3-Clause BSD license. See LICENSE for details
*/

#include "freertos_cpp_util/logging/Log_sink_console.hpp"

bool Log_sink_console::handle_log(String_type* const log)
{
	int ret = printf("%s", log->c_str());
	return ret > 0;
}
