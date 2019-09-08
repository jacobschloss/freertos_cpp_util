/**
 * @brief Logger_types
 * @author Jacob Schloss <jacob@schloss.io>
 * @copyright Copyright (c) 2019 Jacob Schloss. All rights reserved.
 * @license Licensed under the 3-Clause BSD license. See LICENSE for details
*/

#pragma once

#include "common_util/Stack_string.hpp"

#include "freertos_cpp_util/object_pool/Object_pool.hpp"

namespace freertos_util
{
namespace logging
{
	constexpr static size_t NUM_RECORDS = 16;

	typedef Stack_string<128> String_type;
	typedef Object_pool<String_type, NUM_RECORDS> Pool_type;

	enum class LOG_SEVERITY
	{
		FATAL,
		ERROR,
		WARN,
		INFO,
		DEBUG,
		TRACE
	};
}
}
