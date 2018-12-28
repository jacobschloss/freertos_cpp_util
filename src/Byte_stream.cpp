#include "freertos_cpp_util/Byte_stream.hpp"

size_t Byte_stream::read_some_for_ticks(uint8_t* const buf, const size_t read_max, const TickType_t xTicksToWait)
{
	TickType_t ticks_left = xTicksToWait;
	TimeOut_t xTimeOut;
	vTaskSetTimeOutState(&xTimeOut);

	size_t num_read = 0;

	while(
		   (num_read < read_max)
		&& (pdFALSE == xTaskCheckForTimeOut(&xTimeOut, &ticks_left))
		)
	{
		if(m_write_activity.try_take_for_ticks(ticks_left))
		{
			{
				std::lock_guard<Mutex_static> lock(m_read_buf_mutex);
				
				buf[num_read] = m_read_buf.front();
				m_read_buf.pop_front();
			}

			num_read++;
		}
	}

	return num_read;
}