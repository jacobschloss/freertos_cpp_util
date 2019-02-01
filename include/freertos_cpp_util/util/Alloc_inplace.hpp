#pragma once

#include <utility>

///
/// Alloc_inplace
///
/// An allocator that is only useful for calling constructors for in-place construction
/// No memory is actually allocated
/// This is really just a wrapper for placement new, but allows for allocation semantics with constructor forwarding
///
template<class T>
struct Alloc_inplace
{
	typedef T				value_type;
	typedef T*				pointer;
	typedef const T*		const_pointer;
	typedef T&				reference;
	typedef const T&		const_reference;
	typedef std::size_t		size_type;
	typedef std::ptrdiff_t	difference_type;

	template< class U >
	struct rebind
	{
		typedef Alloc_inplace<U> other;
	};

	Alloc_inplace() noexcept
	{

	}

	template< class U >
	Alloc_inplace(const Alloc_inplace<U>& other) noexcept
	{

	}
	~Alloc_inplace()
	{

	}

	//We don't actually give you memory
	pointer allocate(size_type n, const void* hint)
	{
		return nullptr;
	}

	//We don't actually give you memory
	void deallocate(T* p, std::size_t n)
	{

	}

	//We don't actually give you memory
	size_type max_size() const noexcept
	{
		return 0;
	}

	template< class U, class... Args >
	void construct(U* p, Args&&... args)
	{
		::new((void*) p) U(std::forward<Args>(args)...);
	}

	template< class U >
	void destroy(U* p)
	{
		p->~U();
	}
};

template< class T1, class T2 >
bool operator==( const Alloc_inplace<T1>& lhs, const Alloc_inplace<T2>& rhs ) noexcept
{
	return true;
}

template< class T1, class T2 >
bool operator!=( const Alloc_inplace<T1>& lhs, const Alloc_inplace<T2>& rhs ) noexcept
{
	return false;
}
