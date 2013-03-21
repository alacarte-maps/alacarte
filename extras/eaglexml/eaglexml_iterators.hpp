#ifndef EAGLEXML_ITERATORS_HPP_INCLUDED
#define EAGLEXML_ITERATORS_HPP_INCLUDED

// Copyright (C) 2012 Tobias Kahlert
// Version 0.1
//! \file eaglexml_iterators.hpp This file contains eaglexml iterators

#include "eaglexml.hpp"

namespace eaglexml
{
	//! \defgroup Iterators
	//! @{

	//! Iterator of child nodes of xml_node
	template<class Ch_or_CacheType = EAGLEXML_STD_CHAR_TYPE>
	class node_iterator
	{

	public:
		typedef typename internal::extract_char<Ch_or_CacheType>::char_type char_type;

		typedef xml_node<Ch_or_CacheType> value_type;
		typedef xml_node<Ch_or_CacheType> &reference;
		typedef xml_node<Ch_or_CacheType> *pointer;
		typedef std::ptrdiff_t difference_type;
		typedef std::forward_iterator_tag iterator_category;


		node_iterator()
			: m_node(0)
		{
		}

		node_iterator(pointer node)
			: m_node(node->first_node())
		{
		}

		reference operator *() const
		{
			assert(m_node);
			return *m_node;
		}

		pointer operator->() const
		{
			assert(m_node);
			return m_node;
		}

		node_iterator& operator++()
		{
			assert(m_node);
			m_node = m_node->next_sibling();
			return *this;
		}

		node_iterator operator++(int)
		{
			node_iterator tmp = *this;
			++this;
			return tmp;
		}

		bool operator ==(const node_iterator<Ch_or_CacheType> &rhs)
		{
			return m_node == rhs.m_node;
		}

		bool operator !=(const node_iterator<Ch_or_CacheType> &rhs)
		{
			return m_node != rhs.m_node;
		}

	private:

		pointer m_node;
	};

	//! Iterator of child attributes of xml_node
	template<class Ch_or_CacheType = EAGLEXML_STD_CHAR_TYPE>
	class attribute_iterator
	{

	public:
		typedef typename internal::extract_char<Ch_or_CacheType>::char_type char_type;

		typedef xml_attribute<Ch_or_CacheType> value_type;
		typedef xml_attribute<Ch_or_CacheType> &reference;
		typedef xml_attribute<Ch_or_CacheType> *pointer;
		typedef std::ptrdiff_t difference_type;
		typedef std::bidirectional_iterator_tag iterator_category;
		


		attribute_iterator()
			: m_attribute(0)
		{
		}

		attribute_iterator(pointer node)
			: m_attribute(node->first_attribute())
		{
		}

		explicit attribute_iterator(node_iterator<Ch_or_CacheType>& it)
			: m_attribute(it->first_attribute())
		{
		}

		reference operator *() const
		{
			assert(m_attribute);
			return *m_attribute;
		}

		pointer operator->() const
		{
			assert(m_attribute);
			return m_attribute;
		}

		attribute_iterator& operator++()
		{
			assert(m_attribute);
			m_attribute = m_attribute->next_attribute();
			return *this;
		}

		attribute_iterator operator++(int)
		{
			attribute_iterator tmp = *this;
			++this;
			return tmp;
		}

		bool operator ==(const attribute_iterator<Ch_or_CacheType> &rhs)
		{
			return m_attribute == rhs.m_attribute;
		}

		bool operator !=(const attribute_iterator<Ch_or_CacheType> &rhs)
		{
			return m_attribute != rhs.m_attribute;
		}

	private:

		pointer m_attribute;

	};

	//! @}
}

#endif
