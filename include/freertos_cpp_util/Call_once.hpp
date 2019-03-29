/**
 * @author Jacob Schloss <jacob@schloss.io>
 * @copyright Copyright (c) 2018 Jacob Schloss. All rights reserved.
 * @license Licensed under the 3-Clause BSD license. See LICENSE for details
*/

#pragma once

#include "freertos_cpp_util/Mutex_static.hpp"

#include <atomic>
#include <mutex>
#include <version>

class Once_flag;

template<class Callable, class... Args>
void call_once(Once_flag& flag, Callable&& f, Args&&... args);

class Once_flag
{
	protected:

	template<class Callable, class... Args>
	friend void call_once(Once_flag& flag, Callable&& f, Args&&... args);

	std::atomic<bool> m_flag;

	Mutex_static m_mutex;
};

template<class Callable, class... Args>
void call_once(Once_flag& flag, Callable&& f, Args&&... args)
{
	if(!flag.m_flag.load())
	{
		std::lock_guard<Mutex_static> lock(flag.m_mutex);
		
		if(!flag.m_flag.load())
		{
			#if defined(__cpp_lib_invoke) && (__cpp_lib_invoke >= 201411L)
				std::invoke(f, std::forward<Args>(args)...);
			#else
				auto func = std::bind(f, std::forward<Args>(args)...);
				func();
			#endif

			flag.m_flag.store(true);	
		}
	}
}