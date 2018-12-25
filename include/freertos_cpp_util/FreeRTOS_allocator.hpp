/**
 * @brief FreeRTOS C++ custom allocator
 * @author Jacob Schloss <jacob@schloss.io>
 * @copyright Copyright (c) 2018 Jacob Schloss. All rights reserved.
 * @license Licensed under the 3-Clause BSD license. See License for details
*/

#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <new>

#include "FreeRTOS.h"

template<typename T>
class FreeRTOS_allocator
{

	typedef T value_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;

	FreeRTOS_allocator() noexcept
	{

	}

	FreeRTOS_allocator(const FreeRTOS_allocator& rhs) noexcept
	{
		
	}

	template<typename U>
	FreeRTOS_allocator(const FreeRTOS_allocator<U>& rhs) noexcept
	{
		
	}

	~FreeRTOS_allocator()
	{

	}

	template<typename U>
	struct rebind
	{
		typedef FreeRTOS_allocator<U> other;
	};

	pointer address(reference val) const noexcept
	{
		return &val;
	}

	const_pointer address(const_reference val) const noexcept
	{
		return &val;
	}

	size_type max_size() const noexcept
	{
		return std::numeric_limits<std::size_t>::max() / sizeof(T);
	}

	pointer allocate(size_type num, const void* hint = 0)
	{
		return allocate(num);
	}

	pointer allocate(size_type num)
	{
		pointer p = nullptr;
		if(alignof(T) <= portBYTE_ALIGNMENT)
		{
			//default alignment
			p = pvPortMalloc(num * sizeof(T));

			if(p == nullptr)
			{
				throw std::bad_alloc();
			}
		}
		else
		{
			//wider alignment
			constexpr std::uintptr_t allignment = alignof(T);
			constexpr std::uintptr_t allignment_mask = alignof(T) - 1U;

			// constexpr size_t req_allign = alignof(T);
			pointer raw_p = pvPortMalloc(num * sizeof(T) + allignment);

			if(raw_p == nullptr)
			{
				throw std::bad_alloc();
			}

			//get an aligned poiner
			p = (static_cast<std::uintptr_t>(raw_p) & (~allignment_mask)) + allignment;

			//stash the original pointer
			*(reinterpret_cast<void**>(p) - 1) = raw_p;
		}

		return p;
	}

	void construct(pointer p, const T& val)
	{
		::new (p) T(val);
	}

	template< class U, class... Args >
	void construct(U* p, Args&&... args )
	{
		::new (p) U(std::forward<Args>(args)...);
	}

	void destroy(pointer p)
	{
		p->~T();
	}

	void deallocate(pointer p, size_type num)
	{
		if(alignof(T) <= portBYTE_ALIGNMENT)
		{
			//default alignment
			vPortFree(p);
		}
		else
		{
			//wider alignment, we stashed the real heap pointer here
			pointer start = *(reinterpret_cast<void**>(p) - 1);
			vPortFree(start);
		}
	}

};

template< class T1, class T2 >
bool operator==(const FreeRTOS_allocator<T1>& lhs, const FreeRTOS_allocator<T2>& rhs ) noexcept
{
	return true;
}

template< class T1, class T2 >
bool operator!=(const FreeRTOS_allocator<T1>& lhs, const FreeRTOS_allocator<T2>& rhs ) noexcept
{
	return false;
}
