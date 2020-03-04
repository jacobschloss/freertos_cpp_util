/**
 * @brief Global_logger
 * @author Jacob Schloss <jacob@schloss.io>
 * @copyright Copyright (c) 2019 Jacob Schloss. All rights reserved.
 * @license Licensed under the 3-Clause BSD license. See LICENSE for details
*/

#pragma once

#include "freertos_cpp_util/logging/Logger.hpp"

#include <atomic>

namespace freertos_util
{
namespace logging
{

class Global_logger
{
public:

	static Global_logger& get_instance()
	{
		return m_inst;
	}

	static Logger* get()
	{
		return get_instance().m_logger;
	}

	static Logger* set(Logger* const log)
	{
		return get_instance().m_logger = log;
	}

protected:

	Global_logger() : m_logger(nullptr)
	{

	}

	static Global_logger m_inst;

	std::atomic<Logger*> m_logger;
};

}
}
