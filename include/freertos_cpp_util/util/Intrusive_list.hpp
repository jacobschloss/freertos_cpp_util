/**
 * @brief Intrusive doubly linked list 
 * @author Jacob Schloss <jacob@schloss.io>
 * @copyright Copyright (c) 2018 Jacob Schloss. All rights reserved.
 * @license Licensed under the 3-Clause BSD license. See LICENSE for details
*/

#pragma once

#include "freertos_cpp_util/util/Non_copyable.hpp"

#include <cstddef>
#include <type_traits>
#include <iterator>

//An intrusive doubly linked list for general use
//heap allocated

class Intrusive_list;

class Intrusive_list_node
{
public:

	friend class Intrusive_list;

	Intrusive_list_node()
	{
		m_prev = nullptr;
		m_next = nullptr;
	}

	~Intrusive_list_node() = default;

	//copy & assign are default
	Intrusive_list_node(const Intrusive_list_node& rhs) = default;
	Intrusive_list_node& operator=(const Intrusive_list_node& rhs) = default;

	//permit move
	Intrusive_list_node(Intrusive_list_node&& rhs)
	{
		m_prev = rhs.m_prev;
		m_next = rhs.m_next;
		rhs.m_prev = nullptr;
		rhs.m_next = nullptr;
	}

	friend void swap(Intrusive_list_node& lhs, Intrusive_list_node& rhs);

	Intrusive_list_node* prev()
	{
		return m_prev;
	}

	const Intrusive_list_node* prev() const
	{
		return m_prev;
	}

	Intrusive_list_node* next()
	{
		return m_next;
	}

	const Intrusive_list_node* next() const
	{
		return m_next;
	}

protected:
	Intrusive_list_node* m_prev;
	Intrusive_list_node* m_next;
};

//in this list, nodes are held on the stack externally
//lifetime of nodes must be managed by the creator
class Intrusive_list : private Non_copyable
{
public:

	template<typename T>
	class Intrusive_list_iterator : public std::iterator<std::bidirectional_iterator_tag, T>
	{
	public:

		static_assert(std::is_base_of<Intrusive_list_node, T>::value);

		using base = std::iterator<std::bidirectional_iterator_tag, T>;
		using typename base::iterator_category;
		using typename base::value_type;
		using typename base::difference_type;
		using typename base::pointer;
		using typename base::reference;

		Intrusive_list_iterator(pointer x)
		{
			m_curr = x;
		}

		reference operator++()
		{
			m_curr = m_curr->next();
			return *this;
		}
		reference operator--()
		{
			m_curr = m_curr->prev();
			return *this;
		}

		reference operator++(int)
		{
			value_type tmp(*this);
			operator++();
			return tmp;
		}
		reference operator--(int)
		{
			value_type tmp(*this);
			operator--();
			return tmp;
		}

		bool operator==(const reference rhs) const
		{
			return m_curr == rhs.m_curr;
		}
		bool operator!=(const reference rhs) const
		{
			return m_curr != rhs.m_curr;
		}

		reference operator*()
		{
			return *m_curr;
		}
	protected:
		pointer m_curr;
	};

	template<typename T>
	class iterator : public Intrusive_list_iterator<T>
	{
		using base = Intrusive_list_iterator<T>;
		using typename base::iterator_category;
		using typename base::value_type;
		using typename base::difference_type;
		using typename base::pointer;
		using typename base::reference;
	};

	template<typename T>
	class const_iterator : public Intrusive_list_iterator<const T>
	{
		using base = Intrusive_list_iterator<const T>;
		using typename base::iterator_category;
		using typename base::value_type;
		using typename base::difference_type;
		using typename base::pointer;
		using typename base::reference;
	};


	Intrusive_list()
	{
		m_head = nullptr;
		m_tail = nullptr;
	}

	~Intrusive_list() = default;

	//copy & assign are banned
	//Since the nodes are owned externally, it is probably a bad idea to clone the list.
	Intrusive_list(const Intrusive_list& rhs) = delete;
	Intrusive_list& operator=(const Intrusive_list& rhs) = delete;

	//permit move
	Intrusive_list(Intrusive_list&& rhs)
	{
		m_head = rhs.m_head;
		m_tail = rhs.m_tail;
		rhs.m_head = nullptr;
		rhs.m_tail = nullptr;
	}

	template<typename T>
	T* front()
	{
		static_assert(std::is_base_of<Intrusive_list_node, T>::value);

		return static_cast<T*>(m_head);
	}

	template<typename T>
	const T* front() const
	{
		static_assert(std::is_base_of<Intrusive_list_node, T>::value);

		return static_cast<const T*>(m_head);
	}

	template<typename T>
	T* back()
	{
		static_assert(std::is_base_of<Intrusive_list_node, T>::value);

		return static_cast<T*>(m_tail);
	}

	template<typename T>
	const T* back() const 
	{
		static_assert(std::is_base_of<Intrusive_list_node, T>::value);

		return static_cast<const T*>(m_tail);
	}

	template<typename T>
	iterator<T> begin()
	{
		return iterator<T>(m_head);
	}

	template<typename T>
	iterator<T> end()
	{
		return iterator<T>(nullptr);
	}

	template<typename T>
	const_iterator<T> cbegin()
	{
		return const_iterator<T>(m_head);
	}

	template<typename T>
	const_iterator<T> cend()
	{
		return const_iterator<T>(nullptr);
	}

	bool empty() const
	{
		return m_head == nullptr;
	}

	size_t size() const
	{
		size_t count = 0;
		Intrusive_list_node const * n = m_head;
		while(n)
		{
			count++;
			n = n->m_next;
		}

		return count;
	}

	void push_front(Intrusive_list_node* const node)
	{
		if(m_head)
		{
			node->m_next = m_head;
			node->m_prev = nullptr;

			m_head = node;
		}
		else
		{
			node->m_next = nullptr;
			node->m_prev = nullptr;

			m_head = node;
			m_tail = node;
		}	
	}

	void push_back(Intrusive_list_node* const node)
	{
		if(m_tail)
		{
			node->m_next = nullptr;
			node->m_prev = m_tail;

			m_tail = node;
		}
		else
		{
			node->m_next = nullptr;
			node->m_prev = nullptr;

			m_head = node;
			m_tail = node;
		}	
	}

	void pop_front()
	{
		if(m_head)
		{
			if(m_head == m_tail)
			{
				m_head = nullptr;
				m_tail = nullptr;
			}
			else
			{
				m_head = m_head->m_next;
			}
		}
	}

	void pop_back()
	{
		if(m_tail)
		{
			if(m_head == m_tail)
			{
				m_head = nullptr;
				m_tail = nullptr;
			}
			else
			{
				m_tail = m_tail->m_prev;
			}
		}
	}

protected:
	Intrusive_list_node* m_head;
	Intrusive_list_node* m_tail;
};
