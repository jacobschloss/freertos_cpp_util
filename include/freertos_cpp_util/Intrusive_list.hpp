#pragma once

class Intrusive_list_fwd;

class Intrusive_list_fwd_node
{
public:

	friend class Intrusive_list_fwd;

	Intrusive_list_fwd_node()
	{
		m_next = nullptr;
	}

	Intrusive_list_fwd_node* next()
	{
		return m_next;
	}

protected:
	Intrusive_list_fwd_node* m_next;
};

class Intrusive_list_fwd
{
public:

	Intrusive_list_fwd()
	{
		m_head = nullptr;
	}

	template<typename T>
	T* front()
	{
		return static_cast<T*>(m_head);
	}

	bool empty()
	{
		return m_head == nullptr;
	}

	void push_front(Intrusive_list_fwd_node* const node)
	{
		if(m_head)
		{
			node->m_next = m_head;
		}
		else
		{
			node->m_next = nullptr;
		}
		
		m_head = node;
	}

	void pop_front()
	{
		if(m_head)
		{
			m_head = m_head->m_next;
		}
	}

protected:
	Intrusive_list_fwd_node* m_head;
};
