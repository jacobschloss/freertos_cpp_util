/**
 * @brief Stack allocated mutex
 * @author Jacob Schloss <jacob@schloss.io>
 * @copyright Copyright (c) 2025 Jacob Schloss. All rights reserved.
 * @license Licensed under the 3-Clause BSD license. See LICENSE for details
*/

#pragma once

#include "FreeRTOS.h"
#include "event_groups.h"

class Event_group
{
public:
	Event_group()
	{
		m_handle = nullptr;
	}

	void set_handle(const EventGroupHandle_t handle)
	{
		m_handle = handle;
	}

	EventGroupHandle_t get_handle() const
	{
		return m_handle;
	}

	EventBits_t set_bits(const EventBits_t bits_to_set)
	{
		return xEventGroupSetBits(m_handle, bits_to_set);
	}

	EventBits_t clear_bits(const EventBits_t bits_to_clear)
	{
		return xEventGroupClearBits(m_handle, bits_to_clear);
	}

	EventBits_t get_bits()
	{
		return xEventGroupGetBits(m_handle);
	}

	EventBits_t wait_bits(const EventBits_t wait_bits, const bool clear_on_exit, const bool wait_for_all, const TickType_t wait_time)
	{
		return xEventGroupWaitBits(m_handle, wait_bits, clear_on_exit, wait_for_all, wait_time);
	}

	EventBits_t sync(const EventBits_t bits_to_set, const EventBits_t bits_to_wait, const TickType_t wait_time)
	{
		return xEventGroupSync(m_handle, bits_to_set, bits_to_wait, wait_time);
	}

protected:
	EventGroupHandle_t m_handle;
};