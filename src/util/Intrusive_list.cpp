/**
 * @author Jacob Schloss <jacob@schloss.io>
 * @copyright Copyright (c) 2018 Jacob Schloss. All rights reserved.
 * @license Licensed under the 3-Clause BSD license. See LICENSE for details
*/

#include "freertos_cpp_util/util/Intrusive_list.hpp"

void swap(Intrusive_list_node& lhs, Intrusive_list_node& rhs)
{
	using std::swap;

	Intrusive_list_node* const lhs_prev = lhs.prev();
	Intrusive_list_node* const lhs_next = lhs.next();

	Intrusive_list_node* const rhs_prev = rhs.prev();
	Intrusive_list_node* const rhs_next = rhs.next();

	lhs_prev->m_next = &rhs;
	lhs.m_prev = rhs_prev;
	lhs.m_next = rhs_next;
	lhs_next->m_prev = &rhs;

	rhs_prev->m_next = &lhs;
	rhs.m_prev = lhs_prev;
	rhs.m_next = lhs_next;
	rhs_next->m_prev = &lhs;
}