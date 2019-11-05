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

const char* Logger::LOG_LEVEL_to_str(const LOG_LEVEL level)
{
	switch(level)
	{
		case LOG_LEVEL::FATAL:
			return "FATAL";
		case LOG_LEVEL::ERROR:
			return "ERROR";
		case LOG_LEVEL::WARN:
			return "WARN";
		case LOG_LEVEL::INFO:
			return "INFO";
		case LOG_LEVEL::DEBUG:
			return "DEBUG";
		case LOG_LEVEL::TRACE:
			return "TRACE";
		default:
			return "UNKNOWN";
	}

	return "UNKNOWN";
}

void Logger::make_log_element(const char* time_str, LOG_LEVEL level, const char* module_name, const char* msg, String_type* const out_record)
{
	out_record->clear();
	out_record->push_back('[');
	out_record->append(time_str);
	out_record->append("][");
	out_record->append(LOG_LEVEL_to_str(level));
	out_record->append("][");
	out_record->append(module_name);
	out_record->push_back(']');
	out_record->append(msg);

	if(out_record->full())
	{
		out_record->pop_back();
		out_record->pop_back();
	}

	out_record->append("\r\n");	
}

bool Logger::log(const LOG_LEVEL level, const char* module_name, const char* fmt, ...)
{
	if(level > m_sev_mask_level)
	{
		return true;
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

	//verify if this is really an interrupt
	//in some cases eg the USB library will have a code path that is optionally polled or ISR
	if(xPortIsInsideInterrupt() == pdTRUE)
	{
		return log_msg_isr(level, module_name, msg_buf.data());
	}

	return log_msg(level, module_name, msg_buf.data());
}

bool Logger::log_msg(const LOG_LEVEL level, const char* module_name, const char* msg)
{
	if(level > m_sev_mask_level)
	{
		return true;
	}

	//verify if this is really an interrupt
	//in some cases eg the USB library will have a code path that is optionally polled or ISR
	if(xPortIsInsideInterrupt() == pdTRUE)
	{
		return log_msg_isr(level, module_name, msg);
	}

	//get a log buffer or fail
	Pool_type::unique_node_ptr log_element = m_record_pool.allocate_unique();
	if(!log_element)
	{
		m_overflow = true;
		return false;
	}

	Time_str time_str;
	{
		const TickType_t tick_count = xTaskGetTickCount();
		static_assert(sizeof(TickType_t) <= sizeof(uint32_t));
		if(!get_time_str(tick_count, &time_str))
		{
			return false;
		}
	}

	make_log_element(time_str.c_str(), level, module_name, msg, log_element.get());

	//queue for later handling
	if(!m_record_buffer.push_back(log_element.release()))
	{
		return false;
	}

	return true;
}

bool Logger::log_isr(const LOG_LEVEL level, const char* module_name, const char* fmt, ...)
{
	if(level > m_sev_mask_level)
	{
		return true;
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

	return log_msg_isr(level, module_name, msg_buf.data());
}

bool Logger::log_msg_isr(const LOG_LEVEL level, const char* module_name, const char* msg)
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
	
	make_log_element(time_str.c_str(), level, module_name, msg, log_element.get());
	
	//queue for later handling
	if(!m_record_buffer.push_back_isr(log_element.release(), &xHigherPriorityTaskWoken))
	{
		return false;
	}

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