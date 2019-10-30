/**
 * @brief Logger
 * @author Jacob Schloss <jacob@schloss.io>
 * @copyright Copyright (c) 2019 Jacob Schloss. All rights reserved.
 * @license Licensed under the 3-Clause BSD license. See LICENSE for details
*/

#include "common_util/Byte_util.hpp"

#include "freertos_cpp_util/logging/Logger.hpp"
#include "freertos_cpp_util/Task_base.hpp"

#include <cinttypes>
#include <cstdarg>

namespace freertos_util
{
namespace logging
{

bool Logger::get_time_str(const uint32_t tick_count, Time_str* const time_str)
{
	std::array<char, 8+2+1> buf;

	buf[0] = '0';
	buf[1] = 'x';
	Byte_util::u32_to_hex(tick_count, buf.data() + 2);
	buf.back() = '\0';

	time_str->assign(buf.data(), buf.size()-1);

	return true;
}

const char* Logger::LOG_SEVERITY_to_str(const LOG_SEVERITY level)
{
	switch(level)
	{
		case LOG_SEVERITY::FATAL:
			return "FATAL";
		case LOG_SEVERITY::ERROR:
			return "ERROR";
		case LOG_SEVERITY::WARN:
			return "WARN";
		case LOG_SEVERITY::INFO:
			return "INFO";
		case LOG_SEVERITY::DEBUG:
			return "DEBUG";
		case LOG_SEVERITY::TRACE:
			return "TRACE";
		default:
			return "UNKNOWN";
	}

	return "UNKNOWN";
}


/*
bool Logger::log(const LOG_SEVERITY level, const char* module_name, char* fmt, ...)
{
	//cook the string
	std::array<char, 128> msg_buf;
	va_list args;
	va_start (args, fmt);
	int ret = vsnprintf(reinterpret_cast<char*>(msg_buf.data()), msg_buf.size(), fmt, args);
	va_end(args);
	size_t num_to_print = std::min<size_t>(ret, msg_buf.size()-1);	

	if(xPortIsInsideInterrupt() == pdFALSE)
	{
		log(level, module_name, msg_buf);
	}
	else
	{
		log_isr(level, module_name, msg_buf);
	}
}
*/
bool Logger::log(const LOG_SEVERITY level, const char* module_name, char* fmt, ...)
{
	if(level >= m_sev_mask_level)
	{
		return true;
	}

	//get a log buffer or fail
	Pool_type::unique_node_ptr log_element = m_record_pool.allocate_unique();
	if(!log_element)
	{
		m_overflow = true;
		return false;
	}

	//cook the string
	std::array<char, 128> msg_buf;
	va_list args;
	va_start (args, fmt);
	int ret = vsnprintf(reinterpret_cast<char*>(msg_buf.data()), msg_buf.size(), fmt, args);
	va_end(args);
	if(ret < 0)	
	{
		return false;
	}

	const size_t num_to_print = std::min<size_t>(ret, msg_buf.size()-1);

	Time_str time_str;
	{
		const TickType_t tick_count = xTaskGetTickCount();
		static_assert(sizeof(TickType_t) <= sizeof(uint32_t));
		if(!get_time_str(tick_count, &time_str))
		{
			return false;
		}
	}
	
	log_element->push_back('[');
	log_element->append(time_str);
	log_element->push_back(']');
	log_element->push_back('[');
	log_element->append(LOG_SEVERITY_to_str(level));
	log_element->push_back(']');
	log_element->push_back('[');
	log_element->append(module_name);
	log_element->push_back(']');
	log_element->append(msg_buf.data(), num_to_print);
	log_element->append("\r\n");
	
	//queue for later handling
	m_record_buffer.push_back(log_element.release());

	return true;
}

bool Logger::log_isr(const LOG_SEVERITY level, const char* module_name, char* msg)
{
	if(level >= m_sev_mask_level)
	{
		return true;
	}

	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	//get a log buffer or fail
	Pool_type::isr_unique_node_ptr log_element = m_record_pool.allocate_unique_isr(&xHigherPriorityTaskWoken);
	if(!log_element)
	{
		m_overflow = true;
		return false;
	}

	Time_str time_str;
	{
		const TickType_t tick_count = xTaskGetTickCountFromISR();
		static_assert(sizeof(TickType_t) <= sizeof(uint32_t));
		if(!get_time_str(tick_count, &time_str))
		{
			return false;
		}
	}
	
	log_element->push_back('[');
	log_element->append(time_str);
	log_element->push_back(']');
	log_element->push_back('[');
	log_element->append(LOG_SEVERITY_to_str(level));
	log_element->push_back(']');
	log_element->push_back('[');
	log_element->append(module_name);
	log_element->push_back(']');
	log_element->append(msg);
	log_element->append("\r\n");
	
	//queue for later handling
	m_record_buffer.push_back_isr(log_element.release(), &xHigherPriorityTaskWoken);

	//run the scheduler if needed
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

	return true;
}

void Logger::process_one()
{
	//RAII deallocation
	Pool_type::unique_node_ptr log_element;

	//wait for log element
	{
		String_type* log_element_raw = nullptr;
		if(!m_record_buffer.pop_front(&log_element_raw, portMAX_DELAY))
		{
			return;
		}

		log_element.reset(log_element_raw);
	}

	if(m_overflow)
	{
		m_overflow = false;

		String_type overflow_log_element;
		overflow_log_element.assign("\r\nlog storage overflowed\r\n");
		m_sink->handle_log(&overflow_log_element);
	}

	if(m_sink)
	{
		m_sink->handle_log(log_element.get());
	}
}

}
}