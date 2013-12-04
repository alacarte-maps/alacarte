#ifndef EAGLEXML_HPP_INCLUDED
#define EAGLEXML_HPP_INCLUDED



// Copyright (C) 2012 Tobias Kahlert
// Version 0.1
//! \file eaglexml.hpp This file contains eaglexml parser

// If standard library is disabled, user must provide implementations of required functions and typedefs
#if !defined(EAGLEXML_NO_STDLIB)
	#include <cstdlib>      // For std::size_t
	#include <cassert>      // For assert
	#include <new>          // For placement new
	#include <istream>		// For cache
	#include <algorithm>	// For min, max
	#define EAGLEXML_STD_ALLOCATOR(_type) = std::allocator< _type >
	#define EAGLEXML_STD_ISTREAM(_type) = std::basic_istream< _type >
//#define EAGLEXML_STD_
#else
	#define EAGLEXML_STD_ALLOCATOR(_tyoe) 
	#define EAGLEXML_STD_ISTREAM(_type)
#endif

// On MSVC, disable "conditional expression is constant" warning (level 4). 
// This warning is almost impossible to avoid with certain types of templated code
#ifdef _MSC_VER
	#pragma warning(push)
	#pragma warning(disable:4127)   // Conditional expression is constant
#endif

#ifndef EAGLEXML_STD_CHAR_TYPE
#define EAGLEXML_STD_CHAR_TYPE char
#endif


#ifndef EAGLEXML_STD_STACKSEGMENT_SIZE
#define EAGLEXML_STD_STACKSEGMENT_SIZE 1024
#endif




///////////////////////////////////////////////////////////////////////////
// EAGLEXML_PARSE_ERROR
	
#if defined(EAGLEXML_NO_EXCEPTIONS)

#define EAGLEXML_PARSE_ERROR(what, where) { parse_error_handler(what, where); assert(0); }

namespace eaglexml
{
	//! When exceptions are disabled by defining EAGLEXML_NO_EXCEPTIONS, 
	//! this function is called to notify user about the error.
	//! It must be defined by the user.
	//! <br><br>
	//! This function cannot return. If it does, the results are undefined.
	//! <br><br>
	//! A very simple definition might look like that:
	//! <pre>
	//! void %eaglexml::%parse_error_handler(const char *what, void *where)
	//! {
	//!     std::cout << "Parse error: " << what << "\n";
	//!     std::abort();
	//! }
	//! </pre>
	//! \param what Human readable description of the error.
	//! \param where Pointer to character data where error was detected.
	void parse_error_handler(const char *what, void *where);
}

#else
	
#include <exception>    // For std::exception

#define EAGLEXML_PARSE_ERROR(what) throw parse_error(what, require.pos())
#define EAGLEXML_PARSE_ERROR_POS(what, where) throw parse_error(what, where)

namespace eaglexml
{

	//! Parse error exception. 
	//! This exception is thrown by the parser when an error occurs. 
	//! Use what() function to get human-readable error message. 
	//! Use where() function to get a pointer to position within source text where error was detected.
	//! <br><br>
	//! If throwing exceptions by the parser is undesirable, 
	//! it can be disabled by defining EAGLEXML_NO_EXCEPTIONS macro before eaglexml.hpp is included.
	//! This will cause the parser to call eaglexml::parse_error_handler() function instead of throwing an exception.
	//! This function must be defined by the user.
	//! <br><br>
	//! This class derives from <code>std::exception</code> class.
	class parse_error: public std::exception
	{
	
	public:
	
		//! Constructs parse error
		parse_error(const char *what, const char *where)
			: m_what(what)
			, m_where(where)
		{
		}

		//! Gets human readable description of error.
		//! \return Pointer to null terminated description of the error.
		virtual const char *what() const throw()
		{
			return m_what;
		}

		//! Gets pointer to character data where error happened.
		//! Ch should be the same as char type of xml_document that produced the error.
		//! \return Pointer to location within the parsed string where error occured.
		template<class Ch>
		Ch *where() const
		{
			return reinterpret_cast<Ch *>(m_where);
		}

	private:  

		const char *m_what;
		const char *m_where;

	};
}

#endif


namespace eaglexml {
	// Forward declarations
	//! \cond internal
	template<typename Ch_or_CacheType = EAGLEXML_STD_CHAR_TYPE> class xml_node;
	template<typename Ch_or_CacheType = EAGLEXML_STD_CHAR_TYPE> class xml_attribute;
	template<typename Ch_or_CacheType = EAGLEXML_STD_CHAR_TYPE> class xml_document;
	template<typename Ch_or_CacheType = EAGLEXML_STD_CHAR_TYPE> class xml_base;

	namespace internal {
		template<class Ch_or_CacheType = EAGLEXML_STD_CHAR_TYPE, bool impl = false> class xml_parser;
		template<class T> struct extract_char;

	}
	//! \endcond

	//! 
	template<typename CacheType, int Flags, typename Allocator EAGLEXML_STD_ALLOCATOR(internal::extract_char<typename CacheType::char_type>)>
	struct use_cache
	{
		static const int						cache_flags;
		typedef CacheType						cache_type;
		typedef typename CacheType::char_type	char_type;
		typedef Allocator						allocator_type;
	};


	//! Enumeration listing all node types produced by the parser.
	//! Use xml_node::type() function to query node type.
	enum node_type
	{
		node_document,      //!< A document node. Name and value are empty.
		node_element,       //!< An element node. Name contains element name. Value contains text of first data node.
		node_data,          //!< A data node. Name is empty. Value contains data text.
		node_cdata,         //!< A CDATA node. Name is empty. Value contains data text.
		node_comment,       //!< A comment node. Name is empty. Value contains comment text.
		node_declaration,   //!< A declaration node. Name and value are empty. Declaration parameters (version, encoding and standalone) are in node attributes.
		node_doctype,       //!< A DOCTYPE node. Name is empty. Value contains DOCTYPE text.
		node_pi             //!< A PI node. Name contains target. Value contains instructions.
	};

	///////////////////////////////////////////////////////////////////////
	// Parsing flags(RapidXml)

	//! Parse flag instructing the parser to not create data nodes. 
	//! Text of first data node will still be placed in value of parent element, unless eaglexml::parse_no_element_values flag is also specified.
	//! Can be combined with other flags by use of | operator.
	//! <br><br>
	//! See xml_document::parse() function.
	const int parse_no_data_nodes = 0x1;            

	//! Parse flag instructing the parser to not use text of first data node as a value of parent element.
	//! Can be combined with other flags by use of | operator.
	//! Note that child data nodes of element node take precendence over its value when printing. 
	//! That is, if element has one or more child data nodes <em>and</em> a value, the value will be ignored.
	//! Use eaglexml::parse_no_data_nodes flag to prevent creation of data nodes if you want to manipulate data using values of elements.
	//! <br><br>
	//! See xml_document::parse() function.
	const int parse_no_element_values = 0x2;

	//! Parse flag instructing the parser to not place zero terminators after strings in the source text.
	//! By default zero terminators are placed, modifying source text.
	//! Can be combined with other flags by use of | operator.
	//! <br><br>
	//! See xml_document::parse() function.
	const int parse_no_string_terminators = 0x4;

	//! Parse flag instructing the parser to not translate entities in the source text.
	//! By default entities are translated, modifying source text.
	//! Can be combined with other flags by use of | operator.
	//! <br><br>
	//! See xml_document::parse() function.
	const int parse_no_entity_translation = 0x8;

	//! Parse flag instructing the parser to disable UTF-8 handling and assume plain 8 bit characters.
	//! By default, UTF-8 handling is enabled.
	//! Can be combined with other flags by use of | operator.
	//! <br><br>
	//! See xml_document::parse() function.
	const int parse_no_utf8 = 0x10;

	//! Parse flag instructing the parser to create XML declaration node.
	//! By default, declaration node is not created.
	//! Can be combined with other flags by use of | operator.
	//! <br><br>
	//! See xml_document::parse() function.
	const int parse_declaration_node = 0x20;

	//! Parse flag instructing the parser to create comments nodes.
	//! By default, comment nodes are not created.
	//! Can be combined with other flags by use of | operator.
	//! <br><br>
	//! See xml_document::parse() function.
	const int parse_comment_nodes = 0x40;

	//! Parse flag instructing the parser to create DOCTYPE node.
	//! By default, doctype node is not created.
	//! Although W3C specification allows at most one DOCTYPE node, EagleXml will silently accept documents with more than one.
	//! Can be combined with other flags by use of | operator.
	//! <br><br>
	//! See xml_document::parse() function.
	const int parse_doctype_node = 0x80;

	//! Parse flag instructing the parser to create PI nodes.
	//! By default, PI nodes are not created.
	//! Can be combined with other flags by use of | operator.
	//! <br><br>
	//! See xml_document::parse() function.
	const int parse_pi_nodes = 0x100;

	//! Parse flag instructing the parser to validate closing tag names. 
	//! If not set, name inside closing tag is irrelevant to the parser.
	//! By default, closing tags are not validated.
	//! Can be combined with other flags by use of | operator.
	//! <br><br>
	//! See xml_document::parse() function.
	const int parse_validate_closing_tags = 0x200;

	//! Parse flag instructing the parser to trim all leading and trailing whitespace of data nodes.
	//! By default, whitespace is not trimmed. 
	//! This flag does not cause the parser to modify source text.
	//! Can be combined with other flags by use of | operator.
	//! <br><br>
	//! See xml_document::parse() function.
	const int parse_trim_whitespace = 0x400;

	//! Parse flag instructing the parser to condense all whitespace runs of data nodes to a single space character.
	//! Trimming of leading and trailing whitespace of data is controlled by eaglexml::parse_trim_whitespace flag.
	//! By default, whitespace is not normalized. 
	//! If this flag is specified, source text will be modified.
	//! Can be combined with other flags by use of | operator.
	//! <br><br>
	//! See xml_document::parse() function.
	const int parse_normalize_whitespace = 0x800;





	///////////////////////////////////////////////////////////////////////
	// Parsing flags(EagleXml)

	//! Parse flag instructing the parser to create data nodes for content between nodes.
	//! By default only content directly after an element tag is recognized. And of cause data nodes themself.
	const int parse_inter_node_content = 0x1000;



	// Compound flags

	//! Parse flags which represent default behavior of the parser. 
	//! This is always equal to 0, so that all other flags can be simply ored together.
	//! Normally there is no need to inconveniently disable flags by anding with their negated (~) values.
	//! This also means that meaning of each flag is a <i>negation</i> of the default setting. 
	//! For example, if flag name is eaglexml::parse_no_utf8, it means that utf-8 is <i>enabled</i> by default,
	//! and using the flag will disable it.
	//! <br><br>
	//! See xml_document::parse() function.
	const int parse_default = 0;

	//! A combination of parse flags that forbids any modifications of the source text. 
	//! This also results in faster parsing. However, note that the following will occur:
	//! <ul>
	//! <li>names and values of nodes will not be zero terminated, you have to use xml_base::name_size() and xml_base::value_size() functions to determine where name and value ends</li>
	//! <li>entities will not be translated</li>
	//! <li>whitespace will not be normalized</li>
	//! </ul>
	//! See xml_document::parse() function.
	const int parse_non_destructive = parse_no_string_terminators | parse_no_entity_translation;

	//! A combination of parse flags resulting in fastest possible parsing, without sacrificing important data.
	//! <br><br>
	//! See xml_document::parse() function.
	const int parse_fastest = parse_non_destructive | parse_no_data_nodes;

	//! A combination of parse flags resulting in largest amount of data being extracted. 
	//! This usually results in slowest parsing.
	//! <br><br>
	//! See xml_document::parse() function.
	const int parse_full = parse_declaration_node | parse_comment_nodes | parse_doctype_node | parse_pi_nodes | parse_validate_closing_tags;


	///////////////////////////////////////////////////////////////////////
	// Internals

	//! \cond internal
	namespace internal
	{

		// Struct that contains lookup tables for the parser
		// It must be a template to allow correct linking (because it has static data members, which are defined in a header file).
		template<int Dummy>
		struct lookup_tables
		{
			static const unsigned char lookup_whitespace[256];              // Whitespace table
			static const unsigned char lookup_node_name[256];               // Node name table
			static const unsigned char lookup_text[256];                    // Text table
			static const unsigned char lookup_text_pure_no_ws[256];         // Text table
			static const unsigned char lookup_text_pure_with_ws[256];       // Text table
			static const unsigned char lookup_attribute_name[256];          // Attribute name table
			static const unsigned char lookup_attribute_data_1[256];        // Attribute data table with single quote
			static const unsigned char lookup_attribute_data_1_pure[256];   // Attribute data table with single quote
			static const unsigned char lookup_attribute_data_2[256];        // Attribute data table with double quotes
			static const unsigned char lookup_attribute_data_2_pure[256];   // Attribute data table with double quotes
			static const unsigned char lookup_digits[256];                  // Digits
			static const unsigned char lookup_upcase[256];                  // To uppercase conversion table for ASCII characters
		};

		// Find length of the string
		template<class Ch>
		inline std::size_t measure(const Ch *p)
		{
			const Ch *tmp = p;
			while (*tmp) 
				++tmp;
			return tmp - p;
		}

		// Compare strings for equality
		template<class Ch>
		inline bool compare(const Ch *p1, std::size_t size1, const Ch *p2, std::size_t size2, bool case_sensitive)
		{
			if (size1 != size2)
				return false;
			if (case_sensitive)
			{
				for (const Ch *end = p1 + size1; p1 < end; ++p1, ++p2)
					if (*p1 != *p2)
						return false;
			}
			else
			{
				for (const Ch *end = p1 + size1; p1 < end; ++p1, ++p2)
					if (lookup_tables<0>::lookup_upcase[static_cast<unsigned char>(*p1)] != lookup_tables<0>::lookup_upcase[static_cast<unsigned char>(*p2)])
						return false;
			}
			return true;
		}

		template<class Ch>
		inline void copy(const Ch *src, Ch* dest, std::size_t size)
		{
			while(size)
			{
				*(dest++) = *(src++);
				--size;
			}
		}


		template<typename T>
		struct typeinfo
		{
		private:
			template<typename Ty>
			struct _const_tester { static const bool result = false; };
			template<typename Ty>
			struct _const_tester<const Ty> { static const bool result = true; };

			template<typename Ty>
			struct _cacheuse_tester { static const bool result = false; };
			template<typename Ty, int F, typename A>
			struct _cacheuse_tester<use_cache<Ty, F, A> > { static const bool result = true; };

		public:
			static const bool is_const = _const_tester<T>::result;
			static const bool uses_cache = _cacheuse_tester<T>::result;

		};

		template<bool B, typename T>
		struct inherit_if
		{
		};

		template<typename T>
		struct inherit_if<true, T> : public T
		{
		};

		template<typename T>
		struct extract_char
		{
			typedef T char_type;
		};

		template<typename T, int F, typename A>
		struct extract_char<use_cache<T, F, A> >
		{
			typedef typename T::char_type char_type;
		};

		/*template<typename T1, typename T2>
		struct type_checker
		{
			int TypesAreNotEqual[-1];
		};

		template<typename T>
		struct type_checker<T>
		{};*/
	}
	//! \endcond

	//! \defgroup Caches
	//! @{


	//! Interface for a cache. Every cache needs to implement the methods declared in this class
	//!	to work with the eaglexml parser. However it is not needed to inherit from this class,
	//!	because cache types are normally defined by template parameters of xml_document or using use_cache.
	template<typename Ch_or_CacheType = EAGLEXML_STD_CHAR_TYPE>
	class abstract_cache
	{
	public:
		//! \cond internal
		typedef typename internal::extract_char<Ch_or_CacheType>::char_type char_type;
		typedef xml_node<Ch_or_CacheType>									node_type;
		//! \endcond


		struct cache_observer
		{
			typedef typename abstract_cache<Ch_or_CacheType>::char_type			char_type;
			typedef xml_node<Ch_or_CacheType>									node_type;
			
			virtual void on_fetch(unsigned int chars_left, unsigned int need, node_type* active_node) = 0;
		};
	public:

		//! Should load next the part of the xml file into the buffer.
		//! \param chars_left	Number of chars in the buffer not consumed by the parser.
		//! \param need			Minimum number of characters to be loaded.
		//! \param active_node	The node currently parsed. Name, values, attribute names and attribute values must be saved
		//!						and pointers of the node and attributes must be adjusted. Node and attributes themselves
		//!						do not need to be saved, because they were allocated outside the buffer space.
		//! \return				Number of characters available in the buffer to be consumed by the parser.
		//!						Node that this must be greater or equal need.
		virtual unsigned int _fetch(unsigned int chars_left, unsigned int need, node_type* active_node) = 0;

		//! Sould return the end of the buffer.
		//! Note that the returned pointer does not point to a character in the buffer but to the first character located not in the buffer.
		//! cache_end() - 1 however is the last character of the buffer. The cache_end() minus the returned integer of _fetch must
		//! be a pointer to the first character of the buffer!
		virtual	char_type* cache_end() const = 0;
	};


	//! A cache to parse an xml file already loaded into memory.
	//!
	//! This simply takes a buffer containing xml data. Note that you may want to use rapidxml,
	//! which does nearly the same, but offers a lot more features while parsing faster
	//! due to lesser parsing overhead.
	template<typename Ch = EAGLEXML_STD_CHAR_TYPE>
	class static_cache
	{
	public:
		//! \cond internal
		typedef Ch char_type;
		//! \endcond

		typedef typename abstract_cache<Ch>::cache_observer static_cache_observer;
	public:

		//! Constructor for the static cache.
		//!
		//!	\param text The buffer containing the xml data. Must be null terminated!
		//! \param segment_legnth	Size of the segments given to the parser.
		//!							Ideally, this is the size of the xml data.
		static_cache(char_type* text, unsigned int segment_length = 1024 * 8)
			: m_text(text)
			, m_cache_end(text)
			, m_segment_length(segment_length)
			, m_observer(0)
		{
			assert(segment_length);
		}

		//!	Fetches the next segment. 
		//! See abstract_cache::_fetch for more informations
		//! \see{abstract_cache::_fetch}
		unsigned int _fetch(unsigned int chars_to_prepend,
							unsigned int need,
							void*)
		{
			if(m_observer)
				m_observer->on_fetch(chars_to_prepend, need, nullptr);

			// Calculate next end of the cache
			unsigned int next_segment_length = m_segment_length + chars_to_prepend;
			m_cache_end += m_segment_length;

			// Return size of next segment
			return next_segment_length;
		}

		//!	Returns the end of the current segment.
		//! See abstract_cache::cache_end for more informations
		//! \see{abstract_cache::cache_end}
		char_type* cache_end() const
		{
			return m_cache_end;
		}


		static_cache_observer* observer(static_cache_observer* obs)
		{
			return m_observer = obs;
		}

		static_cache_observer* observer() const
		{
			return m_observer;
		}

	private:
		char_type*				m_text;
		char_type*				m_cache_end;
		unsigned int			m_segment_length;
		static_cache_observer	m_observer;
	};


	//! A cache reading the xml data from an input stream.
	//! 
	//! Example for using a stream_cache with std::ifstream.
	//!	\code{.cpp}
	//!		std::ifstream xml_file_stream("example.xml");
	//!		eaglexml::stream_cache<> cache(xml_file_stream);
	//!		eaglexml::xml_document<> document(&cache);
	//!	\endcode
	template<	typename		Ch_or_CacheType = EAGLEXML_STD_CHAR_TYPE,
				typename		StreamType EAGLEXML_STD_ISTREAM(typename internal::extract_char<Ch_or_CacheType>::char_type),
				typename		Allocator EAGLEXML_STD_ALLOCATOR(typename internal::extract_char<Ch_or_CacheType>::char_type) >
	class stream_cache
	{
	public:
		//! \cond internal
		typedef typename internal::extract_char<Ch_or_CacheType>::char_type char_type;
		typedef Allocator								allocator_type;
		typedef StreamType								stream_type;
		typedef xml_node<Ch_or_CacheType>				node_type;
		typedef xml_attribute<Ch_or_CacheType>			attribute_type;
		typedef xml_base<Ch_or_CacheType>				base_type;
		//! \endcond


		struct stream_cache_observer: public abstract_cache<Ch_or_CacheType>::cache_observer
		{
			typedef typename abstract_cache<Ch_or_CacheType>::cache_observer	base;
			typedef typename base::char_type									char_type;
			typedef xml_node<Ch_or_CacheType>									node_type;

			virtual void on_fetch(unsigned int chars_left, unsigned int need, node_type* active_node) = 0;
			virtual void on_buffer_resize() = 0;
			virtual void on_segment_read() = 0;
			virtual void on_read_begin(unsigned int segments) = 0;
		};
	public:
		stream_cache(stream_type& stream, unsigned int buf_size = 1024 * 8, const allocator_type& allocator = std::allocator<char_type>() )
			: m_stream(stream)
			, m_allocator(allocator)
			, m_segment_size(std::max<unsigned int>(buf_size/2, 64))
			, m_buffer_size(std::max<unsigned int>(buf_size, 128))
			, m_max_segments_read(-1)
			, m_resize(true)
			, m_last_saved_node(0)
			, m_content_size(0)
			, m_observer(0)
		{
			m_content_begin = m_buffer = alloc_buffer(m_buffer_size);
		}

		~stream_cache()
		{
			assert(m_buffer);
			dealloc_buffer(m_buffer, m_buffer_size);
		}


		unsigned int segment_size(unsigned int size)
		{
			return m_segment_size = std::max<unsigned int>(size, 16);
		}

		unsigned int segment_size() const
		{
			return m_segment_size;
		}
		
		unsigned int buffer_size() const
		{
			return m_buffer_size;
		}

		unsigned int max_segments_to_read(unsigned int num)
		{
			return m_max_segments_read = num;
		}

		unsigned int max_segments_to_read() const
		{
			return m_max_segments_read;
		}

		bool resize_cache(bool resize)
		{
			return m_resize = resize;
		}

		bool resize_cache() const
		{
			return m_resize;
		}

		stream_cache_observer* observer(stream_cache_observer* obs)
		{
			return m_observer = obs;
		}

		stream_cache_observer* observer() const
		{
			return m_observer;
		}

		//!	Fetches the next segment. 
		//! See abstract_cache::_fetch for more informations
		//! \see{abstract_cache::_fetch}
		unsigned int _fetch(unsigned int reserve, unsigned int need, node_type* activeNode)
		{
			if(m_observer)
				m_observer->on_fetch(reserve, need, activeNode);

			int segments_to_load = calculate_segments_to_laod();


			if(segments_to_load)
			{
				assert(need <= segments_to_load * m_segment_size);

				m_content_begin += m_content_size - reserve;
				m_content_size = reserve;

				// We have enough space to load some more segments
				m_content_size = read_segments(content_end(), segments_to_load) - m_content_begin;

				return m_content_size;
			}

			// Ok we have to backup everything and start at the front
			char_type* rest = content_end() - reserve;

			// backup note
			save_node(activeNode);

			// prepend rest-chars
			internal::copy(rest, content_begin(), reserve);
			m_content_size = reserve;

			// everything should be saved. now load the new segments
			segments_to_load = calculate_segments_to_laod();

			if(!segments_to_load)
			{
				// Ok now our buffer is too small... resize?
				if(!m_resize)
					EAGLEXML_PARSE_ERROR_POS("stream cache is too small!", "stream_cache::_fetch");

				unsigned int old_buffersize = m_buffer_size;

				// Increase size of the buffer
				do {
					m_buffer_size *= 2;
				}
				while(m_buffer_size <= m_segment_size + reserve + need + (m_content_begin - m_buffer));

				if(m_observer)
					m_observer->on_buffer_resize();

				unsigned int offset = backup_end() - buffer_begin();

				// Create new buffer
				char_type* old_buf = m_buffer;
				m_buffer = alloc_buffer(m_buffer_size);

				
				// Set begin to save the node
				m_content_begin = m_buffer;
				save_node(activeNode);

				assert(m_buffer + offset == m_content_begin);
				// Copy reserve data
				internal::copy(old_buf + offset, m_buffer + offset, m_content_size);

				dealloc_buffer(old_buf, old_buffersize);

				segments_to_load = calculate_segments_to_laod();
			}

			assert(need <= segments_to_load * m_segment_size);

			// We have enough space to load some more segments
			m_content_size = read_segments(content_end(), segments_to_load) - m_content_begin;

			return m_content_size;
		}

		//!	Returns the end of the current segment.
		//! See abstract_cache::cache_end for more informations
		//! \see{abstract_cache::cache_end}
		char_type* cache_end() const
		{
			return content_end();
		}

	private:

		unsigned int calculate_segments_to_laod() const
		{
			return std::min<unsigned int>((buffer_end() - content_end()) / m_segment_size, m_max_segments_read);
		}

		char_type* read_segments(char_type* dest, unsigned int num)
		{
			if(m_observer)
				m_observer->on_read_begin(num);

			while(num--)
			{
				m_stream.read(dest, m_segment_size);
				if(m_observer)
					m_observer->on_segment_read();

				unsigned int read = (unsigned int)m_stream.gcount();
				dest += read;

				assert(read <= m_segment_size);
				if(read < m_segment_size && m_stream.eof())
				{
					// Put a 0
					*dest = char_type('\0');
					// imagine we have loaded rest :)
					dest += m_segment_size - read;
					break;
				}
			}

			return dest;
		}


		// save given node to buffer and return next free buffer space
		char_type* save_node(node_type* node)
		{
			if(!node)
				return m_content_begin = buffer_begin();

			if(node == m_last_saved_node)
			{
			//	if(is_in_backup(node->last_attribute()))
			//		return backup_end();
			}

			m_content_begin = buffer_begin();

			// ok we have to save the node
			save_name(node);

			// save attributes
			for(attribute_type* attr = node->first_attribute();
				attr;
				attr = attr->next_attribute())
			{
				save_name(attr);
				save_value(attr);
			}

			save_value(node);

			m_last_saved_node = node;

			return m_content_begin;
		}

		void save_name(base_type* base)
		{
			base->_relocate_name(save_text(base->name(), base->name_size()));
		}

		void save_value(base_type* base)
		{
			base->_relocate_value(save_text(base->value(), base->value_size()));
		}

		char_type* save_text(char_type* txt, unsigned int size)
		{
			if(!size)
				return 0;

			char_type* target = m_content_begin;

			internal::copy(txt, target, size);

			m_content_begin += size;
			*m_content_begin = char_type('\0');
			++m_content_begin;

			return target;
		}


		bool is_in_backup(attribute_type* attr) const
		{
			return attr ? is_in_backup(attr->value()) : false;
		}

		bool is_in_backup(char_type* txt) const
		{
			return (backup_begin() <= txt && txt < backup_end());
		}


		char_type* backup_begin() const
		{
			return buffer_begin();
		}

		char_type* backup_end() const
		{
			return content_begin();
		}

		char_type* content_begin() const
		{
			return m_content_begin;
		}

		char_type* content_end() const
		{
			return content_begin() + m_content_size;
		}

		// Return beginning of the buffer
		char_type* buffer_begin() const
		{
			return m_buffer;
		}

		// return the end of the buffer
		char_type* buffer_end() const
		{
			return m_buffer + m_buffer_size;
		}

		// allocate a new buffer
		char_type* alloc_buffer(unsigned int size)
		{
			return m_allocator.allocate(size);
		}

		// allocate a new buffer
		void dealloc_buffer(char_type* buf, unsigned int num)
		{
			m_allocator.deallocate(buf, num);
		}

	private:
		stream_type&	m_stream;
		bool			m_resize;
		unsigned int	m_max_segments_read;
		unsigned int	m_buffer_size;
		unsigned int	m_segment_size;
		unsigned int	m_content_size;
		char_type*		m_buffer;
		char_type*		m_content_begin;
		allocator_type	m_allocator;
		node_type*		m_last_saved_node;
		stream_cache_observer* m_observer;
	};


	//! @}

	///////////////////////////////////////////////////////////////////////////
	// XML base

	//! Base class for xml_node and xml_attribute implementing common functions: 
	//! name(), name_size(), value(), value_size() and parent().
	//! \param Ch Character type to use
	template<typename Ch_or_CacheType>
	class xml_base
	{
		template<typename C, bool B>
		friend class internal::xml_parser;
	public:
		typedef typename internal::extract_char<Ch_or_CacheType>::char_type char_type;

		///////////////////////////////////////////////////////////////////////////
		// Construction & destruction

	protected:
		// Construct a base with empty name, value and parent
		xml_base(xml_node<Ch_or_CacheType>* parent)
			: m_name(0)
			, m_value(0)
			, m_parent(parent)
		{
		}

		///////////////////////////////////////////////////////////////////////////
		// Node data access

	public:
		//! Gets name of the node. 
		//! Interpretation of name depends on type of node.
		//! Note that name will not be zero-terminated if eaglexml::parse_no_string_terminators option was selected during parse.
		//! <br><br>
		//! Use name_size() function to determine length of the name.
		//! \return Name of node, or empty string if node has no name.
		char_type *name() const
		{
			return m_name ? m_name : nullstr();
		}

		//! Gets size of node name, not including terminator character.
		//! This function works correctly irrespective of whether name is or is not zero terminated.
		//! \return Size of node name, in characters.
		std::size_t name_size() const
		{
			return m_name ? m_name_size : 0;
		}

		//! Gets value of node. 
		//! Interpretation of value depends on type of node.
		//! Note that value will not be zero-terminated if eaglexml::parse_no_string_terminators option was selected during parse.
		//! <br><br>
		//! Use value_size() function to determine length of the value.
		//! \return Value of node, or empty string if node has no value.
		char_type *value() const
		{
			return m_value ? m_value : nullstr();
		}

		//! Gets size of node value, not including terminator character.
		//! This function works correctly irrespective of whether value is or is not zero terminated.
		//! \return Size of node value, in characters.
		std::size_t value_size() const
		{
			return m_value ? m_value_size : 0;
		}

		//! Gets document of which attribute is a child.
		//! \return Pointer to document that contains this attribute, or 0 if there is no parent document.
		xml_document<Ch_or_CacheType> *document() const
		{
			if (xml_node<Ch_or_CacheType> *node = this->parent())
			{
				while (node->parent())
					node = node->parent();
				return node->type() == node_document ? static_cast<xml_document<Ch_or_CacheType> *>(node) : 0;
			}
			else
				return 0;
		}

		///////////////////////////////////////////////////////////////////////////
		// Related nodes access

		//! Gets node parent.
		//! Note that parent will be always 0, if parse_stack_parent_nodes is used
		//! \return Pointer to parent node, or 0 if there is no parent.
		xml_node<Ch_or_CacheType> *parent() const
		{
			return m_parent;
		}

		///////////////////////////////////////////////////////////////////////////
		// Node modification

		//! Relocates the name, while the size of the name remains constant.
		//! This should only be used in caches to backup the active node
		//! \param name the new position of the name.
		void _relocate_name(const char_type *name)
		{
			m_name = const_cast<char_type *>(name);
		}

		//! Relocates the value, while the size of the value remains constant.
		//! This should only be used in caches to backup the active node
		//! \param value the new position of the value.
		void _relocate_value(const char_type *value)
		{
			m_value = const_cast<char_type *>(value);
		}
	private:
		//! Sets name of node to a non zero-terminated string.
		//! See \ref ownership_of_strings.
		//! <br><br>
		//! Note that node does not own its name or value, it only stores a pointer to it. 
		//! It will not delete or otherwise free the pointer on destruction.
		//! It is responsibility of the user to properly manage lifetime of the string.
		//! The easiest way to achieve it is to use memory_pool of the document to allocate the string -
		//! on destruction of the document the string will be automatically freed.
		//! <br><br>
		//! Size of name must be specified separately, because name does not have to be zero terminated.
		//! Use name(const char_type *) function to have the length automatically calculated (string must be zero terminated).
		//! \param name Name of node to set. Does not have to be zero terminated.
		//! \param size Size of name, in characters. This does not include zero terminator, if one is present.
		void name(const char_type *name, std::size_t size)
		{
			m_name = const_cast<char_type *>(name);
			m_name_size = size;
		}

		//! Sets value of node to a non zero-terminated string.
		//! See \ref ownership_of_strings.
		//! <br><br>
		//! Note that node does not own its name or value, it only stores a pointer to it. 
		//! It will not delete or otherwise free the pointer on destruction.
		//! It is responsibility of the user to properly manage lifetime of the string.
		//! The easiest way to achieve it is to use memory_pool of the document to allocate the string -
		//! on destruction of the document the string will be automatically freed.
		//! <br><br>
		//! Size of value must be specified separately, because it does not have to be zero terminated.
		//! Use value(const char_type *) function to have the length automatically calculated (string must be zero terminated).
		//! <br><br>
		//! If an element has a child node of type node_data, it will take precedence over element value when printing.
		//! If you want to manipulate data of elements using values, use parser flag eaglexml::parse_no_data_nodes to prevent creation of data nodes by the parser.
		//! \param value value of node to set. Does not have to be zero terminated.
		//! \param size Size of value, in characters. This does not include zero terminator, if one is present.
		void value(const char_type *value, std::size_t size)
		{
			m_value = const_cast<char_type *>(value);
			m_value_size = size;
		}


	protected:

		// Return empty string
		static char_type *nullstr()
		{
			static char_type zero = char_type('\0');
			return &zero;
		}

		char_type *m_name;                         // Name of node, or 0 if no name
		char_type *m_value;                        // Value of node, or 0 if no value
		std::size_t m_name_size;            // Length of node name, or undefined of no name
		std::size_t m_value_size;           // Length of node value, or undefined if no value
		xml_node<Ch_or_CacheType> *m_parent;             // Pointer to parent node, or 0 if none
	};

	//! Class representing attribute node of XML document. 
	//! Each attribute has name and value strings, which are available through name() and value() functions (inherited from xml_base).
	//! Note that after parse, both name and value of attribute will point to interior of source text used for parsing. 
	//! Thus, this text must persist in memory for the lifetime of attribute.
	//! \param char_type Character type to use.
	template<typename Ch_or_CacheType>
	class xml_attribute: public xml_base<Ch_or_CacheType>
	{
		friend class xml_node<Ch_or_CacheType>;
	public:
		///////////////////////////////////////////////////////////////////////////
		// Types
		typedef xml_base<Ch_or_CacheType>		base;
		typedef typename base::char_type		char_type;

		///////////////////////////////////////////////////////////////////////////
		// Construction & destruction

		//! Constructs an empty attribute with the specified type. 
		//! Consider using memory_pool of appropriate xml_document if allocating attributes manually.
		xml_attribute(xml_node<Ch_or_CacheType>* parent)
			: xml_base<Ch_or_CacheType>(parent)
		{
		}

		///////////////////////////////////////////////////////////////////////////
		// Related nodes access

		//! Gets previous attribute, optionally matching attribute name. 
		//! \param name Name of attribute to find, or 0 to return previous attribute regardless of its name; this string doesn't have to be zero-terminated if name_size is non-zero
		//! \param name_size Size of name, in characters, or 0 to have size calculated automatically from string
		//! \param case_sensitive Should name comparison be case-sensitive; non case-sensitive comparison works properly only for ASCII characters
		//! \return Pointer to found attribute, or 0 if not found.
		xml_attribute<Ch_or_CacheType> *previous_attribute(const char_type *name = 0, std::size_t name_size = 0, bool case_sensitive = true) const
		{
			if (name)
			{
				if (name_size == 0)
					name_size = internal::measure(name);
				for (xml_attribute<Ch_or_CacheType> *attribute = m_prev_attribute; attribute; attribute = attribute->m_prev_attribute)
					if (internal::compare(attribute->name(), attribute->name_size(), name, name_size, case_sensitive))
						return attribute;
				return 0;
			}
			else
				return this->m_parent ? m_prev_attribute : 0;
		}

		//! Gets next attribute, optionally matching attribute name. 
		//! \param name Name of attribute to find, or 0 to return next attribute regardless of its name; this string doesn't have to be zero-terminated if name_size is non-zero
		//! \param name_size Size of name, in characters, or 0 to have size calculated automatically from string
		//! \param case_sensitive Should name comparison be case-sensitive; non case-sensitive comparison works properly only for ASCII characters
		//! \return Pointer to found attribute, or 0 if not found.
		xml_attribute<Ch_or_CacheType> *next_attribute(const char_type *name = 0, std::size_t name_size = 0, bool case_sensitive = true) const
		{
			if (name)
			{
				if (name_size == 0)
					name_size = internal::measure(name);
				for (xml_attribute<Ch_or_CacheType> *attribute = m_next_attribute; attribute; attribute = attribute->m_next_attribute)
					if (internal::compare(attribute->name(), attribute->name_size(), name, name_size, case_sensitive))
						return attribute;
				return 0;
			}
			else
				return this->m_parent ? m_next_attribute : 0;
		}

	private:

		xml_attribute<Ch_or_CacheType> *m_prev_attribute;        // Pointer to previous sibling of attribute, or 0 if none; only valid if parent is non-zero
		xml_attribute<Ch_or_CacheType> *m_next_attribute;        // Pointer to next sibling of attribute, or 0 if none; only valid if parent is non-zero
	};

	///////////////////////////////////////////////////////////////////////////
	// XML node

	//! Class representing a node of XML document. 
	//! Each node may have associated name and value strings, which are available through name() and value() functions. 
	//! Interpretation of name and value depends on type of the node.
	//! Type of node can be determined by using type() function.
	//! <br><br>
	//! Note that after parse, both name and value of node, if any, will point interior of source text used for parsing. 
	//! Thus, this text must persist in the memory for the lifetime of node.
	//! \param char_type Character type to use.
	template<typename Ch_or_CacheType>
	class xml_node: public xml_base<Ch_or_CacheType>
	{
		template<typename C, bool B>
		friend class internal::xml_parser;
	protected:


		///////////////////////////////////////////////////////////////////////////
		// Construction & destruction

		//! Constructs an empty node with the specified type. 
		//! Consider using memory_pool of appropriate document to allocate nodes manually.
		//! \param type Type of node to construct.
		xml_node(node_type type, xml_node* parent, internal::xml_parser<Ch_or_CacheType>* parser)
			: base(parent)
			, m_type(type)
			, m_first_attribute(0)
			, m_parser(parser)
		{
		}


	public:

		///////////////////////////////////////////////////////////////////////////
		// Types
		typedef xml_base<Ch_or_CacheType>		base;
		typedef typename base::char_type		char_type;


	public:
		///////////////////////////////////////////////////////////////////////////
		// Node data access

		//! Gets type of node.
		//! \return Type of node.
		node_type type() const
		{
			return m_type;
		}

		///////////////////////////////////////////////////////////////////////////
		// Related nodes access

		//! Gets first child node, optionally matching node name.
		//! \param name Name of child to find, or 0 to return first child regardless of its name; this string doesn't have to be zero-terminated if name_size is non-zero
		//! \param name_size Size of name, in characters, or 0 to have size calculated automatically from string
		//! \param case_sensitive Should name comparison be case-sensitive; non case-sensitive comparison works properly only for ASCII characters
		//! \return Pointer to found child, or 0 if not found.
		//!
		//! \attention Due to this call all attributes, names and values are reset!
		//! \attention Nodes skipped by this call will not be reachable anymore!
		//! \attention All child nodes of this node will become inconsistent!
		xml_node *first_node(const char_type *name = 0, std::size_t name_size = 0, bool case_sensitive = true)
		{
			assert(m_parser);

			// Reset all node data
#ifndef EAGLEXML_NO_NODE_RESET
			this->_relocate_name(0);
			this->_relocate_value(0);
			this->m_first_attribute = 0;
			this->m_last_attribute = 0;
#endif
			xml_node *child = m_parser->parse_child(this);
			if (child && name)
			{
				if (name_size == 0)
					name_size = internal::measure(name);

				while(child)
				{
					if(internal::compare(child->name(), child->name_size(), name, name_size, case_sensitive))
						return child;

					child = m_parser->parse_next(child);
				}

				return 0;
			}
			else
				return child;
		}


		//! Gets next sibling node, optionally matching node name. 
		//! Behavior is undefined if node has no parent.
		//! Use parent() to test if node has a parent.
		//! \param name Name of sibling to find, or 0 to return next sibling regardless of its name; this string doesn't have to be zero-terminated if name_size is non-zero
		//! \param name_size Size of name, in characters, or 0 to have size calculated automatically from string
		//! \param case_sensitive Should name comparison be case-sensitive; non case-sensitive comparison works properly only for ASCII characters
		//! \return Pointer to found sibling, or 0 if not found.
		//!
		//! \attention Due to this call this node will become inconsistent!
		//! \attention Nodes skipped by this call will not be reachable anymore!
		//! \attention All child nodes of this node will become inconsistent!
		xml_node *next_sibling(const char_type *name = 0, std::size_t name_size = 0, bool case_sensitive = true)
		{
			assert(m_parser);

			// If this is the document, there will be no next sibling.
			if(!this->parent())
				return 0;

			// Reset all node data
#ifndef EAGLEXML_NO_NODE_RESET
			this->_relocate_name(0);
			this->_relocate_value(0);
			this->m_first_attribute = 0;
			this->m_last_attribute = 0;
#endif

			// Backup parser
			internal::xml_parser<Ch_or_CacheType>* parser = m_parser;

			// After this call, this node is certainly inconsistent, unless next is null. Then this node remains consistent
			xml_node *next = parser->parse_next(this);

			if(name && next)
			{
				if (name_size == 0)
					name_size = internal::measure(name);

				while(next)
				{
					if(internal::compare(next->name(), next->name_size(), name, name_size, case_sensitive))
						return next;

					next = parser->parse_next(next);
				}

				return 0;
			}else
				return next;

		}

		//! Gets first attribute of node, optionally matching attribute name.
		//! \param name Name of attribute to find, or 0 to return first attribute regardless of its name; this string doesn't have to be zero-terminated if name_size is non-zero
		//! \param name_size Size of name, in characters, or 0 to have size calculated automatically from string
		//! \param case_sensitive Should name comparison be case-sensitive; non case-sensitive comparison works properly only for ASCII characters
		//! \return Pointer to found attribute, or 0 if not found.
		xml_attribute<Ch_or_CacheType> *first_attribute(const char_type *name = 0, std::size_t name_size = 0, bool case_sensitive = true) const
		{
			if (name)
			{
				if (name_size == 0)
					name_size = internal::measure(name);
				for (xml_attribute<Ch_or_CacheType> *attribute = m_first_attribute; attribute; attribute = attribute->m_next_attribute)
					if (internal::compare(attribute->name(), attribute->name_size(), name, name_size, case_sensitive))
						return attribute;
				return 0;
			}
			else
				return m_first_attribute;
		}

		//! Gets last attribute of node, optionally matching attribute name.
		//! \param name Name of attribute to find, or 0 to return last attribute regardless of its name; this string doesn't have to be zero-terminated if name_size is non-zero
		//! \param name_size Size of name, in characters, or 0 to have size calculated automatically from string
		//! \param case_sensitive Should name comparison be case-sensitive; non case-sensitive comparison works properly only for ASCII characters
		//! \return Pointer to found attribute, or 0 if not found.
		xml_attribute<Ch_or_CacheType> *last_attribute(const char_type *name = 0, std::size_t name_size = 0, bool case_sensitive = true) const
		{
			if (name)
			{
				if (name_size == 0)
					name_size = internal::measure(name);
				for (xml_attribute<Ch_or_CacheType> *attribute = m_last_attribute; attribute; attribute = attribute->m_prev_attribute)
					if (internal::compare(attribute->name(), attribute->name_size(), name, name_size, case_sensitive))
						return attribute;
				return 0;
			}
			else
				return m_first_attribute ? m_last_attribute : 0;
		}

		///////////////////////////////////////////////////////////////////////////
		// Node modification
	protected:

		//! Prepends a new attribute to the node.
		//! \param attribute Attribute to prepend.
		void prepend_attribute(xml_attribute<Ch_or_CacheType> *attribute)
		{
			assert(attribute);
			if (first_attribute())
			{
				attribute->m_next_attribute = m_first_attribute;
				m_first_attribute->m_prev_attribute = attribute;
			}
			else
			{
				attribute->m_next_attribute = 0;
				m_last_attribute = attribute;
			}
			m_first_attribute = attribute;
			attribute->m_parent = this;
			attribute->m_prev_attribute = 0;
		}

		//! Appends a new attribute to the node.
		//! \param attribute Attribute to append.
		void append_attribute(xml_attribute<Ch_or_CacheType> *attribute)
		{
			assert(attribute);
			if (first_attribute())
			{
				attribute->m_prev_attribute = m_last_attribute;
				m_last_attribute->m_next_attribute = attribute;
			}
			else
			{
				attribute->m_prev_attribute = 0;
				m_first_attribute = attribute;
			}
			m_last_attribute = attribute;
			attribute->m_parent = this;
			attribute->m_next_attribute = 0;
		}

	private:

		///////////////////////////////////////////////////////////////////////////
		// Restrictions

		// No copying
		xml_node(const xml_node &);
		void operator =(const xml_node &);

		///////////////////////////////////////////////////////////////////////////
		// Data members

		// Note that some of the pointers below have UNDEFINED values if certain other pointers are 0.
		// This is required for maximum performance, as it allows the parser to omit initialization of 
		// unneeded/redundant values.
		//
		// The rules are as follows:
		// 1. first_node and first_attribute contain valid pointers, or 0 if node has no children/attributes respectively
		// 2. last_node and last_attribute are valid only if node has at least one child/attribute respectively, otherwise they contain garbage
		// 3. prev_sibling and next_sibling are valid only if node has a parent, otherwise they contain garbage

		node_type m_type;                       // Type of node; always valid
		xml_attribute<Ch_or_CacheType> *m_first_attribute;   // Pointer to first attribute of node, or 0 if none; always valid
		xml_attribute<Ch_or_CacheType> *m_last_attribute;    // Pointer to last attribute of node, or 0 if none; this value is only valid if m_first_attribute is non-zero
	protected:
		internal::xml_parser<Ch_or_CacheType> *m_parser;
	};



	///////////////////////////////////////////////////////////////////////////
	// XML document

	//! This class represents root of the DOM hierarchy. 
	//! It is also an xml_node and a memory_pool through public inheritance.
	//! Use parse() function to build a DOM tree from a zero-terminated XML text string.
	//! parse() function allocates memory for nodes and attributes by using functions of xml_document, 
	//! which are inherited from memory_pool.
	//! To access root node of the document, use the document itself, as if it was an xml_node.
	//! \param Ch Character type to use.
	template<typename Ch> // Ch_or_CacheType is here the char_type
	class xml_document: public xml_node<Ch>
	{
	public:
		////////////////////////////////////////////////////////////////////////
		// Types
		typedef xml_node<Ch>				base;
		typedef	typename base::char_type	char_type;

		//! Constructs empty XML document
		xml_document()
			: base(node_document, 0, 0)
		{
		}

		~xml_document()
		{
			if(this->m_parser)
			{
				delete this->m_parser;
			}
		}

		template<int Flags, typename CacheType>
		void parse(CacheType* cache)
		{
			assert(cache);
			this->m_parser = new internal::xml_parser<use_cache<CacheType, Flags, std::allocator<void> >, true>(cache, this, std::allocator<void>());
		}


		template<int Flags, typename CacheType, typename Allocator>
		void parse(CacheType* cache, const Allocator& allocator)
		{
			this->m_parser = new internal::xml_parser<use_cache<CacheType, Flags, Allocator>, true>(cache, this, allocator);
		}
	};

	template<typename CacheType, int Flags, typename Allocator> // Ch_or_CacheType is here the char_type
	class xml_document<use_cache<CacheType, Flags, Allocator> >: public xml_node<use_cache<CacheType, Flags, Allocator> >
	{
	public:
		////////////////////////////////////////////////////////////////////////
		// Types
		typedef xml_node<use_cache<CacheType, Flags, Allocator> >	base;
		typedef	typename base::char_type							char_type;



		xml_document()
			: base(node_document, 0, 0)
		{
		}

		~xml_document()
		{
			if(this->m_parser)
			{
				delete this->m_parser;
			}
		}

		template<int Dump>
		void parse(CacheType* cache)
		{
			assert(!this->m_parser);
			this->m_parser = new internal::xml_parser<use_cache<CacheType, Flags, std::allocator<void> >, false>(cache, this, Allocator());
		}

		template<int Dump>
		void parse(CacheType* cache, const Allocator& allocator)
		{
			assert(!this->m_parser);
			this->m_parser = new internal::xml_parser<use_cache<CacheType, Flags, Allocator>, false>(cache, this, allocator);
		}
	};


	//! \cond internal
	namespace internal {


	//////////////////////////////////////////////////
	// xml_parser
	template<typename Ch, bool Impl>
	class xml_parser
	{
	public:
		typedef xml_node<Ch>		xml_node_type;
		typedef xml_attribute<Ch>	xml_attribute_type;
		typedef Ch					char_type;
	public:
		virtual ~xml_parser() {}
		virtual xml_node_type*	parse_next(xml_node_type* cur) = 0;
		virtual xml_node_type*	parse_child(xml_node_type* cur) = 0;
	};

	template<typename CacheType, int Flags_, typename Allocator, bool Impl>
	struct parser_init //impl = false
	{
		// If this is inherited, the parser has no virtual methods!
		typedef CacheType												cache_type;
		typedef xml_node<use_cache<CacheType, Flags_, Allocator> >		xml_node_type;
		typedef xml_attribute<use_cache<CacheType, Flags_, Allocator> >	xml_attribute_type;
		typedef typename CacheType::char_type							char_type;
		typedef Allocator												allocator_type;
		static const int												parser_flags = Flags_;
	};

	template<typename CacheType, int Flags_, typename Allocator>
	struct parser_init<CacheType, Flags_, Allocator, true>
		:public xml_parser<typename CacheType::char_type>
	{
		typedef CacheType														cache_type;
		typedef Allocator														allocator_type;
		typedef typename xml_parser<typename CacheType::char_type>::xml_node_type		xml_node_type;
		typedef typename xml_parser<typename CacheType::char_type>::xml_attribute_type	xml_attribute_type;
		typedef typename xml_parser<typename CacheType::char_type>::char_type			char_type;
		static const int parser_flags = Flags_;
	};


	template<typename CacheType, int Flags_, typename Allocator, bool impl>
	 class xml_parser<use_cache<CacheType, Flags_, Allocator>, impl>
		:public parser_init<CacheType, Flags_, Allocator, impl>
	{
	public:
		typedef parser_init<CacheType, Flags_, Allocator, impl>			base;
		typedef typename base::cache_type								cache_type;
		typedef typename base::allocator_type							allocator_type;
		typedef typename base::xml_node_type							xml_node_type;
		typedef typename base::xml_attribute_type						xml_attribute_type;
		typedef typename base::char_type								char_type;

		using base::parser_flags;


		xml_parser(CacheType* cache, xml_node_type* root, const allocator_type& allocator)
			: require(cache)
			, m_nextstatus(next_parse_node)
			, m_allocator(allocator)
		{
			m_bottomnode = m_topnode = root;

			parse_bom();

			// Parse until first <
			skip<whitespace_pred, parser_flags>();

			if(*require.pos() != char_type('<'))
				EAGLEXML_PARSE_ERROR("expected document to begin with '<'");
			require.absorb(1);
		}

		xml_node_type* parse_next(xml_node_type* cur)
		{
			assert(cur != m_bottomnode);

			xml_node_type* parent = cur->parent();

			/*while(m_topnode != cur || (m_nextstatus != next_close_node && m_nextstatus != next_parse_endtag))
			{
				assert(m_topnode != m_bottomnode);

				parse_according_to_status<parser_flags>();
			}

			close_current_tag<parser_flags>();*/

			while(m_topnode != parent)
			{
				parse_according_to_status<parser_flags>();
			}

			xml_node_type* node;

			// continue to parse, until we get a valid node or reach the end-tag or the end of the file
			do {

				if ((m_topnode == parent && m_nextstatus == next_parse_endtag) || m_nextstatus == next_eof)
					return 0;

				node = parse_according_to_status<parser_flags>();
				
			}while(!node);

			return node;
		}

		xml_node_type* parse_child(xml_node_type* cur)
		{
			assert(cur == m_topnode);
			
			if(m_nextstatus != next_parse_node)
				return 0;

			// Ok we definitely have a child here
			xml_node_type* node;
			do
			{
				node = parse_according_to_status<parser_flags>();

			} while(!node && (m_nextstatus == next_parse_content || m_nextstatus == next_parse_node) && m_nextstatus != next_eof);

			return node;
		}


	private:

		// Parse BOM, if any
		void parse_bom()
		{
			// UTF-8?
			char_type* text = require(3);
			if (static_cast<unsigned char>(text[0]) == 0xEF && 
				static_cast<unsigned char>(text[1]) == 0xBB && 
				static_cast<unsigned char>(text[2]) == 0xBF)
			{
				require.absorb(3);      // Skip utf-8 bom
			}
		}

		///////////////////////////////////////////////////////////////////////
		// Internal character utility functions

		// Detect whitespace character
		struct whitespace_pred
		{
			static unsigned char test(char_type ch)
			{
				return internal::lookup_tables<0>::lookup_whitespace[static_cast<unsigned char>(ch)];
			}
		};

		// Detect node name character
		struct node_name_pred
		{
			static unsigned char test(char_type ch)
			{
				return internal::lookup_tables<0>::lookup_node_name[static_cast<unsigned char>(ch)];
			}
		};

		// Detect attribute name character
		struct attribute_name_pred
		{
			static unsigned char test(char_type ch)
			{
				return internal::lookup_tables<0>::lookup_attribute_name[static_cast<unsigned char>(ch)];
			}
		};

		// Detect text character (PCDATA)
		struct text_pred
		{
			static unsigned char test(char_type ch)
			{
				return internal::lookup_tables<0>::lookup_text[static_cast<unsigned char>(ch)];
			}
		};

		// Detect text character (PCDATA) that does not require processing
		struct text_pure_no_ws_pred
		{
			static unsigned char test(char_type ch)
			{
				return internal::lookup_tables<0>::lookup_text_pure_no_ws[static_cast<unsigned char>(ch)];
			}
		};

		// Detect text character (PCDATA) that does not require processing
		struct text_pure_with_ws_pred
		{
			static unsigned char test(char_type ch)
			{
				return internal::lookup_tables<0>::lookup_text_pure_with_ws[static_cast<unsigned char>(ch)];
			}
		};

		// Detect attribute value character
		template<char_type Quote>
		struct attribute_value_pred
		{
			static unsigned char test(char_type ch)
			{
				if (Quote == char_type('\''))
					return internal::lookup_tables<0>::lookup_attribute_data_1[static_cast<unsigned char>(ch)];
				if (Quote == char_type('\"'))
					return internal::lookup_tables<0>::lookup_attribute_data_2[static_cast<unsigned char>(ch)];
				return 0;       // Should never be executed, to avoid warnings on Comeau
			}
		};

		// Detect attribute value character
		template<char_type Quote>
		struct attribute_value_pure_pred
		{
			static unsigned char test(char_type ch)
			{
				if (Quote == char_type('\''))
					return internal::lookup_tables<0>::lookup_attribute_data_1_pure[static_cast<unsigned char>(ch)];
				if (Quote == char_type('\"'))
					return internal::lookup_tables<0>::lookup_attribute_data_2_pure[static_cast<unsigned char>(ch)];
				return 0;       // Should never be executed, to avoid warnings on Comeau
			}
		};

		// Insert coded character, using UTF8 or 8-bit ASCII
		template<int Flags>
		static void insert_coded_character(char_type *&text, unsigned long code)
		{
			if (Flags & parse_no_utf8)
			{
				// Insert 8-bit ASCII character
				// Todo: possibly verify that code is less than 256 and use replacement char otherwise?
				text[0] = static_cast<unsigned char>(code);
				text += 1;
			}
			else
			{
				// Insert UTF8 sequence
				if (code < 0x80)    // 1 byte sequence
				{
					text[0] = static_cast<unsigned char>(code);
					text += 1;
				}
				else if (code < 0x800)  // 2 byte sequence
				{
					text[1] = static_cast<unsigned char>((code | 0x80) & 0xBF); code >>= 6;
					text[0] = static_cast<unsigned char>(code | 0xC0);
					text += 2;
				}
				else if (code < 0x10000)    // 3 byte sequence
				{
					text[2] = static_cast<unsigned char>((code | 0x80) & 0xBF); code >>= 6;
					text[1] = static_cast<unsigned char>((code | 0x80) & 0xBF); code >>= 6;
					text[0] = static_cast<unsigned char>(code | 0xE0);
					text += 3;
				}
				else if (code < 0x110000)   // 4 byte sequence
				{
					text[3] = static_cast<unsigned char>((code | 0x80) & 0xBF); code >>= 6;
					text[2] = static_cast<unsigned char>((code | 0x80) & 0xBF); code >>= 6;
					text[1] = static_cast<unsigned char>((code | 0x80) & 0xBF); code >>= 6;
					text[0] = static_cast<unsigned char>(code | 0xF0);
					text += 4;
				}
				else    // Invalid, only codes up to 0x10FFFF are allowed in Unicode
				{
					EAGLEXML_PARSE_ERROR_POS("invalid numeric character entity", text);
				}
			}
		}

		// Skip characters until predicate evaluates to true
		template<class StopPred, int Flags>
		void skip()
		{
			unsigned int steps;
			char_type *tmp, *end;
			while (1)
			{
				tmp = require.requireRest(&steps);
				end = tmp + steps;
				while(tmp < end)
				{
					if(!StopPred::test(*tmp))
					{
						require.absorb(steps - (end-tmp));
						return;
					}
					++tmp;
				}
				require.absorb(steps);
			}
		}

		// Skip characters until predicate evaluates to true
		template<char_type StopCh, int Flags>
		void skipToChar()
		{
			unsigned int steps;
			char_type *tmp, *end;
			while (1)
			{
				tmp = require.requireRest(&steps);
				end = tmp + steps;
				while(tmp < end)
				{
					if(*tmp == StopCh)
					{
						require.absorb(steps - (end-tmp));
						return;
					}

					if (!*tmp)
						EAGLEXML_PARSE_ERROR("unexpected end of data");
					++tmp;
				}
				require.absorb(steps);
			}
		}

		// Skip characters until predicate evaluates to true while doing the following:
		// - replacing XML character entity references with proper characters (&apos; &amp; &quot; &lt; &gt; &#...;)
		// - condensing whitespace sequences to single space character
		template<class StopPred, class StopPredPure, int Flags>
		char_type *skip_and_expand_character_refs()
		{
			assert(require.sequence());

			// If entity translation, whitespace condense and whitespace trimming is disabled, use plain skip
			if (Flags & parse_no_entity_translation && 
				!(Flags & parse_normalize_whitespace) &&
				!(Flags & parse_trim_whitespace))
			{
				skip<StopPred, Flags>();
				return require.pos();
			}

			skip<StopPredPure, Flags>();

			// Use simple skip until first modification is detected. Then safe offset
			int offset = require.pos() - require.sequence();

			// Then skip everything until end. After that, the hole text should be in the cache
			skip<StopPred, Flags>();
			char_type *end = require.pos();


			// Use translation skip
			char_type *src = require.sequence() + offset;
			char_type *dest = src;

			while (src < end)
			{
				// If entity translation is enabled    
				if (!(Flags & parse_no_entity_translation))
				{
					// Test if replacement is needed
					if (src[0] == char_type('&'))
					{
						switch (src[1])
						{

							// &amp; &apos;
						case char_type('a'): 
							if (src[2] == char_type('m') && src[3] == char_type('p') && src[4] == char_type(';'))
							{
								*dest = char_type('&');
								++dest;
								src += 5;
								continue;
							}
							if (src[2] == char_type('p') && src[3] == char_type('o') && src[4] == char_type('s') && src[5] == char_type(';'))
							{
								*dest = char_type('\'');
								++dest;
								src += 6;
								continue;
							}
							break;

							// &quot;
						case char_type('q'): 
							if (src[2] == char_type('u') && src[3] == char_type('o') && src[4] == char_type('t') && src[5] == char_type(';'))
							{
								*dest = char_type('"');
								++dest;
								src += 6;
								continue;
							}
							break;

							// &gt;
						case char_type('g'): 
							if (src[2] == char_type('t') && src[3] == char_type(';'))
							{
								*dest = char_type('>');
								++dest;
								src += 4;
								continue;
							}
							break;

							// &lt;
						case char_type('l'): 
							if (src[2] == char_type('t') && src[3] == char_type(';'))
							{
								*dest = char_type('<');
								++dest;
								src += 4;
								continue;
							}
							break;

							// &#...; - assumes ASCII
						case char_type('#'): 
							if (src[2] == char_type('x'))
							{
								unsigned long code = 0;
								src += 3;   // Skip &#x
								while (1)
								{
									unsigned char digit = internal::lookup_tables<0>::lookup_digits[static_cast<unsigned char>(*src)];
									if (digit == 0xFF)
										break;
									code = code * 16 + digit;
									++src;
								}
								insert_coded_character<Flags>(dest, code);    // Put character in output
							}
							else
							{
								unsigned long code = 0;
								src += 2;   // Skip &#
								while (1)
								{
									unsigned char digit = internal::lookup_tables<0>::lookup_digits[static_cast<unsigned char>(*src)];
									if (digit == 0xFF)
										break;
									code = code * 10 + digit;
									++src;
								}
								insert_coded_character<Flags>(dest, code);    // Put character in output
							}
							if (*src == char_type(';'))
								++src;
							else
								EAGLEXML_PARSE_ERROR_POS("expected ;", src);
							continue;

							// Something else
						default:
							// Ignore, just copy '&' verbatim
							break;

						}
					}
				}

				// If whitespace condensing is enabled
				if (Flags & parse_normalize_whitespace)
				{
					// Test if condensing is needed                 
					if (whitespace_pred::test(*src))
					{
						*dest = char_type(' '); ++dest;    // Put single space in dest
						++src;                      // Skip first whitespace char
						// Skip remaining whitespace chars
						while (whitespace_pred::test(*src))
							++src;
						continue;
					}
				}

				// No replacement, only copy character
				*dest++ = *src++;

			}

			// Return new end
			return dest;

		}

		///////////////////////////////////////////////////////////////////////
		// Internal parsing functions


		// Parse XML declaration (<?xml...)
		template<int Flags>
		xml_node_type *parse_xml_declaration()
		{
			char_type* text;
			// If parsing of declaration is disabled
			if (!(Flags & parse_declaration_node))
			{
				// Skip until end of declaration
				while (1)
				{
					skipToChar<char_type('?'), Flags>();
					text = require(2);
					if(text[1] == char_type('>'))
						break;

					require.absorb(1);
				}
				require.absorb(2);    // Skip '?>'
				m_nextstatus = next_parse_content;
				return 0;
			}

			// Create declaration
			xml_node_type *declaration = buyNodeOnStack(node_declaration);

			// Skip whitespace before attributes or ?>
			skip<whitespace_pred, Flags>();

			// Parse declaration attributes
			parse_node_attributes<Flags>(declaration);

			// Skip ?>
			text = require(2);
			if (text[0] != char_type('?') || text[1] != char_type('>'))
				EAGLEXML_PARSE_ERROR("expected ?>");
			require.absorb(2);


			m_nextstatus = next_close_node;

			return declaration;
		}

		// Parse XML comment (<!--...)
		template<int Flags>
		xml_node_type *parse_comment()
		{
			char_type* text;
			// If parsing of comments is disabled
			if (!(Flags & parse_comment_nodes))
			{
				// Skip until end of comment
				while (1)
				{
					skipToChar<char_type('-'), Flags>();
					text = require(3);
					if(text[1] == char_type('-') && text[2] == char_type('>'))
						break;

					require.absorb(1);
				}
				require.absorb(3);	// Skip '-->'
				m_nextstatus = next_parse_content;
				return 0;			// Do not produce comment node
			}

			// Remember value start
			require.beginSequence();

			// Skip until end of comment
			while (1)
			{
				skipToChar<char_type('-'), Flags>();
				text = require(3);
				if(text[1] == char_type('-') && text[2] == char_type('>'))
					break;

				require.absorb(1);
			}

			// Create comment node
			text = require.endSequence();

			xml_node_type *comment = buyNodeOnStack(node_comment);
			comment->value(text, require.pos() - text);

			// Place zero terminator after comment value
			if (!(Flags & parse_no_string_terminators))
				text[comment->value_size()] = char_type('\0');

			require.absorb(3);	// Skip '-->'
			m_nextstatus = next_close_node;
			return comment;
		}

		// Parse DOCTYPE
		template<int Flags>
		xml_node_type *parse_doctype()
		{
			// Remember value start, but only if we have to remember
			if (Flags & parse_doctype_node)
				require.beginSequence();

			char_type* text;

			// Skip to >
			while (*(text  = require(3)) != char_type('>'))
			{
				// Determine character type
				switch (*text)
				{

					// If '[' encountered, scan for matching ending ']' using naive algorithm with depth
					// This works for all W3C test files except for 2 most wicked
				case char_type('['):
					{
						text = require.absorb(1);     // Skip '['
						int depth = 1;
						while (depth > 0)
						{
							text = require(1);
							switch (*text)
							{
							case char_type('['): ++depth; break;
							case char_type(']'): --depth; break;
							case 0: EAGLEXML_PARSE_ERROR("unexpected end of data");
							}
							require.absorb(1);
						}
						break;
					}

					// Error on end of text
				case char_type('\0'):
					EAGLEXML_PARSE_ERROR("unexpected end of data");

					// Other character, skip it
				default:
					require.absorb(1);

				}
			}

			// If DOCTYPE nodes enabled
			if (Flags & parse_doctype_node)
			{
				char_type* value = require.endSequence();
				// Create a new doctype node
				xml_node_type *doctype = buyNodeOnStack(node_doctype);
				
				text = require.pos();
				doctype->value(value, text - value);

				// Place zero terminator after value
				if (!(Flags & parse_no_string_terminators))
					*text = char_type('\0');

				require.absorb(1);      // skip '>'
				m_nextstatus = next_close_node;
				return doctype;
			}
			else
			{
				require.absorb(1);      // skip '>'
				m_nextstatus = next_parse_content;
				return 0;
			}

		}

		// Parse PI
		template<int Flags>
		xml_node_type *parse_pi()
		{
			char_type *text;

			// If creation of PI nodes is enabled
			if (Flags & parse_pi_nodes)
			{
				// Create pi node
				xml_node_type *pi = buyNodeOnStack(node_pi);

				// Extract PI target name
				require.beginSequence();
				skip<node_name_pred, Flags>();

				char_type* name = require.endSequence();
				if (name == require.pos())
					EAGLEXML_PARSE_ERROR("expected PI target");

				pi->name(name, require.pos()- name);

				// Skip whitespace between pi target and pi
				skip<whitespace_pred, Flags>();

				// Remember start of pi
				require.beginSequence();

				// Skip to '?>'
				while (1)
				{
					skipToChar<char_type('?'), Flags>();
					text = require(2);
					if(text[1] == char_type('>'))
						break;

					require.absorb(1);
				}

				name = require.endSequence();
				// Set pi value (verbatim, no entity expansion or whitespace normalization)
				pi->value(name, require.pos()- name);     

				// Place zero terminator after name and value
				if (!(Flags & parse_no_string_terminators))
				{
					pi->name()[pi->name_size()] = char_type('\0');
					pi->value()[pi->value_size()] = char_type('\0');
				}

				require.absorb(2);              // Skip '?>'
				m_nextstatus = next_close_node;
				return pi;
			}
			else
			{
				// Skip to '?>'
				while (1)
				{
					skipToChar<char_type('?'), Flags>();
					text = require(2);
					if(text[1] == char_type('>'))
						break;
					require.absorb(1);
				}
				require.absorb(2);    // Skip '?>'
				m_nextstatus = next_parse_content;
				return 0;
			}
		}


		// Parse CDATA
		template<int Flags>
		xml_node_type *parse_cdata()
		{
			char_type* text;

			// If CDATA is disabled
			if (Flags & parse_no_data_nodes)
			{
				// Skip until end of cdata
				while (1)
				{
					skipToChar<char_type(']'), Flags>();
					text = require(3);
					if(text[1] == char_type(']') && text[2] == char_type('>'))
						break;

					require.absorb(1);
				}
				require.absorb(3);      // Skip ]]>
				m_nextstatus = next_parse_content;
				return 0;       // Do not produce CDATA node
			}

			// Skip until end of cdata
			require.beginSequence();
			while (1)
			{
				skipToChar<char_type(']'), Flags>();
				text = require(3);
				if(text[1] == char_type(']') && text[2] == char_type('>'))
					break;

				require.absorb(1);
			}

			// Create new cdata node
			xml_node_type *cdata = buyNodeOnStack(node_cdata);

			char_type *value = require.endSequence();
			cdata->value(value, require.pos() - value);

			// Place zero terminator after value
			if (!(Flags & parse_no_string_terminators))
				*text = char_type('\0');

			require.absorb(3);     // Skip ]]>
			m_nextstatus = next_close_node;
			return cdata;
		}

		// Parse element node
		template<int Flags>
		xml_node_type *parse_element()
		{
			// Create element node
			xml_node_type *element = buyNodeOnStack(node_element);

			// Extract element name
			require.beginSequence();
			skip<node_name_pred, Flags>();

			char_type* name = require.endSequence();
			char_type* text = require.pos();
			unsigned int name_size = text - name;
			if (text == name)
				EAGLEXML_PARSE_ERROR("expected element name");
			element->name(name, name_size);

			// Skip whitespace between element name and attributes or >
			skip<whitespace_pred, Flags>();

			// Parse attributes, if any
			parse_node_attributes<Flags>(element);

			// Determine ending type
			text = require(2);
			if (*text == char_type('>'))
			{
				require.absorb(1);

				// Save name
				if (Flags & parse_validate_closing_tags)
					m_tagstack.push(element->name(), element->name_size());

				parse_content<Flags>(element);
			}
			else if (*text == char_type('/'))
			{
				m_nextstatus = next_close_node;
				text = require.absorb(1);
				if (*text != char_type('>'))
					EAGLEXML_PARSE_ERROR("expected >");
				require.absorb(1);
			}
			else
				EAGLEXML_PARSE_ERROR("expected >");

			// Place zero terminator after name
			if (!(Flags & parse_no_string_terminators))
				element->name()[element->name_size()] = char_type('\0');

			// Return parsed element
			return element;
		}


		template<int Flags>
		xml_node_type *parse_according_to_status()
		{
			switch(m_nextstatus)
			{
			case next_close_data_node:
				close_data_node<Flags>();
				break;

			case next_close_node:
				close_node<Flags>();
				break;

			case next_parse_endtag:
				parse_endtag<Flags>();
				break;

			case next_parse_node:
				return parse_node<Flags>();

			case next_parse_content:
				return parse_content<Flags>(0);

			case next_eof:
			case next_illigal_status:
				assert(0);
				break;
			}

			return 0;
		}

		// Determine node type, and parse it
		template<int Flags>
		xml_node_type *parse_node()
		{
			char_type* text = require(9);
			assert(*text != char_type('<'));

			m_nextstatus = next_illigal_status;

			// Parse proper node type
			switch (text[0])
			{

				// ...
			default: 
				// Parse and append element node
				return parse_element<Flags>();

				// ?...

			case char_type('/'):
				m_nextstatus = next_parse_endtag;
				require.absorb(1); // skip /
				return 0;
			case char_type('?'):
				if ((text[1] == char_type('x') || text[1] == char_type('X')) &&
					(text[2] == char_type('m') || text[2] == char_type('M')) && 
					(text[3] == char_type('l') || text[3] == char_type('L')) &&
					whitespace_pred::test(text[4]))
				{
					// '<?xml ' - xml declaration
					require.absorb(5);      // Skip '?xml '
					return parse_xml_declaration<Flags>();
				}
				else
				{
					//Skip '?'
					require.absorb(1);

					// Parse PI
					return parse_pi<Flags>();
				}

				// !...
			case char_type('!'): 

				// Parse proper subset of <! node
				switch (text[1])    
				{

					// !-
				case char_type('-'):
					if (text[2] == char_type('-'))
					{
						// '<!--' - xml comment
						require.absorb(3);     // Skip '!--'
						return parse_comment<Flags>();
					}
					break;

					// <![
				case char_type('['):
					if (text[2] == char_type('C') && text[3] == char_type('D') && text[4] == char_type('A') && 
						text[5] == char_type('T') && text[6] == char_type('A') && text[7] == char_type('['))
					{
						// '<![CDATA[' - cdata
						require.absorb(8);     // Skip '![CDATA['
						return parse_cdata<Flags>();
					}
					break;

					// <!D
				case char_type('D'):
					if (text[2] == char_type('O') && text[3] == char_type('C') && text[4] == char_type('T') && 
						text[5] == char_type('Y') && text[6] == char_type('P') && text[7] == char_type('E') && 
						whitespace_pred::test(text[8]))
					{
						// '<!DOCTYPE ' - doctype
						require.absorb(9);      // skip '!DOCTYPE '
						return parse_doctype<Flags>();
					}

				default:
					break;
				}   // switch

				// Attempt to skip other, unrecognized node types starting with <!
				require.absorb(1);	// Skip !

				// skip to >
				skipToChar<char_type('>'), Flags>();

				require.absorb(1);     // Skip '>'
				return 0;   // No node recognized

			}
		}

		template<int Flags>
		void close_node()
		{
			assert(m_topnode);
			// we have to pop the current node
			m_topnode = m_topnode->parent();
			m_nodestack.sell();

			// Next thing is content
			m_nextstatus = next_parse_content;
		}

		template<int Flags>
		void parse_endtag()
		{
			// we have to parse the end-tag; validate it; and pop current node
			if (Flags & parse_validate_closing_tags)
			{
				// Skip and validate closing tag name
				require.beginSequence();
				skip<node_name_pred, Flags>();
				char_type* closing_name = require.endSequence();

				if (!internal::compare(m_tagstack.top(), m_tagstack.top_size(), closing_name, require.pos() - closing_name, true))
					EAGLEXML_PARSE_ERROR("invalid closing tag name");
				m_tagstack.pop();
			}
			else
			{
				// No validation, just skip name
				skip<node_name_pred, Flags>();
			}
			// Skip remaining whitespace after node name
			skip<whitespace_pred, Flags>();
			if (*require(1) != char_type('>'))
				EAGLEXML_PARSE_ERROR("expected >");
			require.absorb(1); // skip ">"
		
			// Continue to close the tag
			close_node<Flags>();
		}

		template<int Flags>
		void close_data_node()
		{
			close_node<Flags>();

			if(*require(1) == char_type('\0'))
			{
				if(m_topnode->parent() != m_bottomnode)
					EAGLEXML_PARSE_ERROR("unexpected end of data");

				m_nextstatus = next_eof;
				return;
			}

			m_nextstatus = next_parse_node;
		}

		template<int Flags>
		void close_current_tag()
		{
			// This function should only be called if there is something to be closed
			switch(m_nextstatus)
			{
			case next_parse_endtag:
				parse_endtag<Flags>();
				break;

			case next_close_node:
				close_node<Flags>();
				break;

			case next_close_data_node:
				close_data_node<Flags>();
				break;

			default:
				assert(0);
			}
		}

		// Parse contents of the node - children, data etc.
		template<int Flags>
		xml_node_type* parse_content(xml_node_type *node)
		{
			char_type* text;
			char_type ch;
			xml_node_type *data = 0;

			if(node || (Flags & parse_inter_node_content))
			{
				// Skip whitespace between > and node contents

				if(Flags & parse_trim_whitespace)
					skip<whitespace_pred, Flags>();

				require.beginSequence();

				char_type* end;
				if (Flags & parse_normalize_whitespace)
					end = skip_and_expand_character_refs<text_pred, text_pure_with_ws_pred, Flags>();   
				else
					end = skip_and_expand_character_refs<text_pred, text_pure_no_ws_pred, Flags>();

				// require now to handle string terminator later
				char_type* value = require.endSequence();


				// Trim trailing whitespace if flag is set; leading was already trimmed by whitespace skip after >
				if (Flags & parse_trim_whitespace && end > value)
				{
					if (Flags & parse_normalize_whitespace)
					{
						// Whitespace is already condensed to single space characters by skipping function, so just trim 1 char off the end
						if (*(end - 1) == char_type(' '))
							--end;
					}
					else
					{
						// Backup until non-whitespace character is found
						while (whitespace_pred::test(*(end-1)))
							--end;
					}
				}


				// Create new data node
				if (!(Flags & parse_no_data_nodes) && !node && end != value)
				{
					data = buyNodeOnStack(node_data);
					data->value(value, end - value);
				}

				// Add data to parent node if no data exists yet
				if (!(Flags & parse_no_element_values) && node) 
					node->value(value, end - value);

				ch = *require.pos();

				// Place zero terminator after value
				if (!(Flags & parse_no_string_terminators))
				{
					*end = char_type('\0');
				}

				// require 2 for the switch inclusive the terminator!
				text = require(2);

			}else{
				// Skip content
				skip<text_pred, Flags>();
				text = require(2);
				ch = *require.pos();
				assert(ch == char_type('\0') || ch == char_type('<'));

			}

			// Determine what comes next: node closing, child node, data node, or 0?
			switch (ch)
			{

				// Node closing or child node
			case char_type('<'):
				if(data)
				{
					require.absorb(1);     // Skip '<'
					m_nextstatus = next_close_data_node;
				}
				else if (text[1] == char_type('/'))
				{
					m_nextstatus = next_parse_endtag;
					require.absorb(2);	// skip </
				}
				else
				{
					// Child node
					require.absorb(1);     // Skip '<'
					m_nextstatus = next_parse_node;
				}
				break;

				// End of data - error
			case char_type('\0'):
				assert(m_topnode);
				if ((data && m_topnode->parent() != m_bottomnode) || (!data && m_topnode != m_bottomnode))
					EAGLEXML_PARSE_ERROR("unexpected end of data");
				m_nextstatus = data? next_close_data_node : next_eof;
				break;
			default:
					EAGLEXML_PARSE_ERROR("expected child-node or end-tag");
			}

			return data;
		}

		// Parse XML attributes of the node
		template<int Flags>
		void parse_node_attributes(xml_node_type *node)
		{
			// arrange space for new attributes
			m_attrbuffer.clear();

			// For all attributes
			char_type* text;
			while (1)
			{
				text = require(1);
				if(!attribute_name_pred::test(*text))
					break;
				// Extract attribute name
				require.beginSequence();
				require.absorb(1); // Skip first character of attribute name
				skip<attribute_name_pred, Flags>();
				
				char_type* name = require.endSequence();

				// Create new attribute
				xml_attribute_type *attribute = buyAttribute(node);
				attribute->name(name, require(0) - name);
				node->append_attribute(attribute);

				// Skip whitespace after attribute name
				skip<whitespace_pred, Flags>();

				// Skip =
				text = require(1);
				if (*text != char_type('='))
					EAGLEXML_PARSE_ERROR("expected =");
				require.absorb(1);

				// Add terminating zero after name
				if (!(Flags & parse_no_string_terminators))
					attribute->name()[attribute->name_size()] = 0;

				// Skip whitespace after =
				skip<whitespace_pred, Flags>();

				// Skip quote and remember if it was ' or "
				text = require(2);
				char_type quote = *text;
				if (quote != char_type('\'') && quote != char_type('"'))
					EAGLEXML_PARSE_ERROR("expected ' or \"");
				require.absorb(1);

				// Extract attribute value and expand char refs in it
				require.beginSequence();
				char_type *end;
				const int AttFlags = Flags & ~parse_normalize_whitespace;   // No whitespace normalization in attributes
				if (quote == char_type('\''))
					end = skip_and_expand_character_refs<attribute_value_pred<char_type('\'')>, attribute_value_pure_pred<char_type('\'')>, AttFlags>();
				else
					end = skip_and_expand_character_refs<attribute_value_pred<char_type('"')>, attribute_value_pure_pred<char_type('"')>, AttFlags>();

				char_type *value = require.endSequence();
				// Set attribute value
				attribute->value(value, end - value);

				// Make sure that end quote is present
				if (*require.pos() != quote)
					EAGLEXML_PARSE_ERROR("expected ' or \"");
				require.absorb(1);

				// Add terminating zero after value
				if (!(Flags & parse_no_string_terminators))
					attribute->value()[attribute->value_size()] = 0;

				// Skip whitespace after attribute value
				skip<whitespace_pred, Flags>();
			}
		}

	private:

		enum Status
		{
			next_parse_content,
			next_parse_node,
			next_parse_endtag,
			next_close_node,
			next_close_data_node,
			next_eof,
			next_illigal_status
		};

		xml_node_type* buyNodeOnStack(node_type type)
		{
			// buy new node on stack
			xml_node_type* newnode = m_nodestack.buy();

			// init new node.
			new(newnode) xml_node_type(type, m_topnode, this);
			
			// new node is now top of the stack
			m_topnode = newnode;
			require.setActiveNode(newnode);

			return newnode;
		}


		xml_attribute_type* buyAttribute(xml_node_type* parent)
		{
			xml_attribute_type* attr = m_attrbuffer.buy();
			new(attr) xml_attribute_type(parent);

			return attr;
		}


		template<typename T, std::size_t Size = EAGLEXML_STD_STACKSEGMENT_SIZE>
		class Stack
		{
			struct StackSegment
			{
				/*StackSegment()
					: prev(0)
					, next(0)
				{
				}*/

				void init()
				{
					prev = 0;
					next = 0;
				}


				/*inline T* content() const
				{
					return 
				}*/

				StackSegment* prev;
				T content[Size];
				StackSegment* next;
			};
		public:

			Stack()
				: m_top(0)
				, m_size(0)
			{
				m_bottom = m_segment = allocSegment();
			}

			~Stack()
			{
				StackSegment* segment = m_bottom;
				while(segment)
				{
					StackSegment* toDelete = segment;
					segment = segment->next;
					allocator.destroy(toDelete);
					allocator.deallocate(toDelete, Size);
				}
			}

			T* buy()
			{
				if(m_top >= Size)
				{
					if(!m_segment->next)
					{
						// Alloc new segment
						StackSegment* newsegment = allocSegment();
						m_segment->next = newsegment;
						newsegment->prev = m_segment;
					}
					m_segment = m_segment->next;
					m_top = 0;
				}
				++m_size;
				return m_segment->content + (m_top++);
			}

			T* top()
			{
				return  m_segment->content + m_top - 1;
			}


			T* sell()
			{
				if(m_top <= 0)
				{
					m_segment = m_segment->prev;
					assert(m_segment);
					m_top = Size;
				}
				--m_size;
				return  m_segment->content + (--m_top - 1);
			}

			std::size_t size() const
			{
				return m_size;
			}

			void clear()
			{
				m_top = 0;
				m_segment = m_bottom;
			}

		private:
			StackSegment* allocSegment()
			{
				StackSegment* segment = allocator.allocate(1);
				segment->init();
				return segment;
			}

			std::size_t m_size;
			std::size_t m_top;
			StackSegment* m_segment;
			StackSegment* m_bottom;
			typename Allocator::template rebind<StackSegment>::other allocator;
		};

		template<typename T, std::size_t Size = EAGLEXML_STD_STACKSEGMENT_SIZE>
		class StringStack
		{
			struct StackSegment
			{
				StackSegment()
					: prev(0)
					, next(0)
					, m_top(0)
				{
				}

				StackSegment* prev;
				T content[Size];
				StackSegment* next;
				std::size_t m_top;

			};
		public:

			StringStack()
			{
				m_bottom = m_segment = allocSegment();
			}

			~StringStack()
			{
				StackSegment* segment = m_bottom;
				while(segment)
				{
					StackSegment* toDelete = segment;
					segment = segment->next;
					allocator.destroy(toDelete);
					allocator.deallocate(toDelete, Size);
				}
			}

			void push(const T* str, std::size_t size)
			{
				std::size_t need = size + sizeof(std::size_t);
				assert(need <= Size);

				if(m_segment->m_top + need > Size)
				{
					if(!m_segment->next)
					{
						// Alloc new segment
						StackSegment* newsegment = allocSegment();
						m_segment->next = newsegment;
						newsegment->prev = m_segment;
					}
					m_segment = m_segment->next;
					m_segment->m_top = 0;
				}

				eaglexml::internal::copy(str, m_segment->content + m_segment->m_top, size);
				m_segment->m_top += size;
				*reinterpret_cast<std::size_t*>(m_segment->content + m_segment->m_top) = size;
				m_segment->m_top += sizeof(std::size_t);
			}

			void pop()
			{
				if(m_segment->m_top <= 0)
				{
					m_segment = m_segment->prev;
					assert(m_segment);
				}

				m_segment->m_top -= sizeof(std::size_t) + top_size();
			}

			T* top() const
			{
				T* t = m_segment->content + (m_segment->m_top - sizeof(std::size_t));

				std::size_t size = *reinterpret_cast<std::size_t*>(t);

				t -= size;

				return t;
			}

			std::size_t top_size() const
			{
				T* t = m_segment->content + (m_segment->m_top - sizeof(std::size_t));

				return *reinterpret_cast<std::size_t*>(t);
			}

		private:
			StackSegment* allocSegment()
			{
				StackSegment* segment = allocator.allocate(1);
				new(segment) StackSegment();
				return segment;
			}


			//std::size_t m_size;
			StackSegment* m_segment;
			StackSegment* m_bottom;
			typename Allocator::template rebind<StackSegment>::other allocator;
		};


		class Absorber
		{
		public:
			Absorber(cache_type* cache)
				: m_cache(cache)
				, m_chars_left(0)
				, m_active_node(0)
				, m_next(0)
				, m_sequence(0)
			{
				assert(cache);
				(*this)(1);
			}

			char_type* operator ()(unsigned int need)
			{
				assert(need >= 0);
				if(need <= m_chars_left)
					return m_next;

				char_type* next = m_next;

				// Calculate data already parsed for the sequence
				unsigned int skip_seq = m_sequence? m_next - m_sequence : 0;

				unsigned int sequence_start_befor_cache_end = m_sequence? m_cache->cache_end() - m_sequence : 0;

				// - _fetch
				// @param	number of chars to be prepeded to the new page
				// @param	number of chars needed in the next page
				// @param	current node, whose attributes and own values must be saved
				// @ret		number of chars in the new buffer
				unsigned int size = m_cache->_fetch(std::max(sequence_start_befor_cache_end, m_chars_left), need, m_active_node);
				
				if(size < need)
					assert(size >= need);
		
				char_type* cacheEnd		= m_cache->cache_end();
				char_type* cacheBegin	= cacheEnd - size;
				char_type* cacheNext	= cacheBegin + skip_seq;
				assert(cacheEnd);

				if(m_sequence)
				{
					m_sequence = cacheBegin;
				}
				m_chars_left = cacheEnd - cacheNext;
				m_next = cacheNext;


				return m_next;
			}

			char_type* requireRest(unsigned int* count)
			{
				if(m_chars_left == 0)
				{
					(*this)(1);
				}
				assert(count);
				*count = m_chars_left;
				return m_next;
			}

			char_type* pos() const
			{
				return m_next;
			}


			char_type* absorb(unsigned int n)
			{
				if(n > m_chars_left)
					assert(n <= m_chars_left);
				m_chars_left -= n;
				return m_next += n;
			}

			void setActiveNode(xml_node_type* node)
			{
				m_active_node = node;
			}

			void resettActiveNode()
			{
				m_active_node = 0;
			}

			char_type* sequence() const
			{
				return m_sequence;
			}

			void beginSequence()
			{
				assert(!m_sequence);
				m_sequence = m_next;
			}

			char_type* endSequence()
			{
				char_type* seq = m_sequence;
				m_sequence = 0;
				return seq;
			}

		private:
			xml_node_type*	m_active_node;
			char_type*		m_next;
			char_type*		m_sequence;
			unsigned int	m_chars_left;
			cache_type*		m_cache;
		};

		Absorber					require;
		StringStack<char_type>		m_tagstack;
		Stack<xml_node_type>		m_nodestack;
		Stack<xml_attribute_type>	m_attrbuffer;
		allocator_type				m_allocator;
		xml_node_type*				m_topnode;
		xml_node_type*				m_bottomnode;
		Status						m_nextstatus;
	};

	}
	//! \endcond


	//! \cond internal
	namespace internal
	{

		// Whitespace (space \n \r \t)
		template<int Dummy>
		const unsigned char lookup_tables<Dummy>::lookup_whitespace[256] = 
		{
			// 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
			0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  0,  0,  1,  0,  0,  // 0
			0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 1
			1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 2
			0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 3
			0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 4
			0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 5
			0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 6
			0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 7
			0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 8
			0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 9
			0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // A
			0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // B
			0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // C
			0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // D
			0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // E
			0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0   // F
		};

		// Node name (anything but space \n \r \t / > ? \0)
		template<int Dummy>
		const unsigned char lookup_tables<Dummy>::lookup_node_name[256] = 
		{
			// 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
			0,  1,  1,  1,  1,  1,  1,  1,  1,  0,  0,  1,  1,  0,  1,  1,  // 0
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 1
			0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0,  // 2
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0,  0,  // 3
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 4
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 5
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 6
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 7
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 8
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 9
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // A
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // B
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // C
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // D
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // E
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1   // F
		};

		// Text (i.e. PCDATA) (anything but < \0)
		template<int Dummy>
		const unsigned char lookup_tables<Dummy>::lookup_text[256] = 
		{
			// 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
			0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 0
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 1
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 2
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0,  1,  1,  1,  // 3
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 4
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 5
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 6
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 7
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 8
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 9
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // A
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // B
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // C
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // D
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // E
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1   // F
		};

		// Text (i.e. PCDATA) that does not require processing when ws normalization is disabled 
		// (anything but < \0 &)
		template<int Dummy>
		const unsigned char lookup_tables<Dummy>::lookup_text_pure_no_ws[256] = 
		{
			// 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
			0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 0
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 1
			1,  1,  1,  1,  1,  1,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 2
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0,  1,  1,  1,  // 3
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 4
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 5
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 6
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 7
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 8
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 9
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // A
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // B
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // C
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // D
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // E
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1   // F
		};

		// Text (i.e. PCDATA) that does not require processing when ws normalizationis is enabled
		// (anything but < \0 & space \n \r \t)
		template<int Dummy>
		const unsigned char lookup_tables<Dummy>::lookup_text_pure_with_ws[256] = 
		{
			// 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
			0,  1,  1,  1,  1,  1,  1,  1,  1,  0,  0,  1,  1,  0,  1,  1,  // 0
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 1
			0,  1,  1,  1,  1,  1,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 2
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0,  1,  1,  1,  // 3
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 4
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 5
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 6
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 7
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 8
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 9
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // A
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // B
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // C
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // D
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // E
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1   // F
		};

		// Attribute name (anything but space \n \r \t / < > = ? ! \0)
		template<int Dummy>
		const unsigned char lookup_tables<Dummy>::lookup_attribute_name[256] = 
		{
			// 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
			0,  1,  1,  1,  1,  1,  1,  1,  1,  0,  0,  1,  1,  0,  1,  1,  // 0
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 1
			0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0,  // 2
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0,  0,  0,  0,  // 3
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 4
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 5
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 6
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 7
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 8
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 9
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // A
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // B
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // C
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // D
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // E
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1   // F
		};

		// Attribute data with single quote (anything but ' \0)
		template<int Dummy>
		const unsigned char lookup_tables<Dummy>::lookup_attribute_data_1[256] = 
		{
			// 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
			0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 0
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 1
			1,  1,  1,  1,  1,  1,  1,  0,  1,  1,  1,  1,  1,  1,  1,  1,  // 2
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 3
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 4
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 5
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 6
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 7
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 8
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 9
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // A
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // B
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // C
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // D
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // E
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1   // F
		};

		// Attribute data with single quote that does not require processing (anything but ' \0 &)
		template<int Dummy>
		const unsigned char lookup_tables<Dummy>::lookup_attribute_data_1_pure[256] = 
		{
			// 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
			0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 0
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 1
			1,  1,  1,  1,  1,  1,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  // 2
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 3
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 4
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 5
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 6
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 7
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 8
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 9
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // A
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // B
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // C
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // D
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // E
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1   // F
		};

		// Attribute data with double quote (anything but " \0)
		template<int Dummy>
		const unsigned char lookup_tables<Dummy>::lookup_attribute_data_2[256] = 
		{
			// 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
			0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 0
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 1
			1,  1,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 2
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 3
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 4
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 5
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 6
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 7
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 8
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 9
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // A
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // B
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // C
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // D
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // E
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1   // F
		};

		// Attribute data with double quote that does not require processing (anything but " \0 &)
		template<int Dummy>
		const unsigned char lookup_tables<Dummy>::lookup_attribute_data_2_pure[256] = 
		{
			// 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
			0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 0
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 1
			1,  1,  0,  1,  1,  1,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 2
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 3
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 4
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 5
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 6
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 7
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 8
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 9
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // A
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // B
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // C
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // D
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // E
			1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1   // F
		};

		// Digits (dec and hex, 255 denotes end of numeric character reference)
		template<int Dummy>
		const unsigned char lookup_tables<Dummy>::lookup_digits[256] = 
		{
			// 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
			255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  // 0
			255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  // 1
			255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  // 2
			0,  1,  2,  3,  4,  5,  6,  7,  8,  9,255,255,255,255,255,255,  // 3
			255, 10, 11, 12, 13, 14, 15,255,255,255,255,255,255,255,255,255,  // 4
			255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  // 5
			255, 10, 11, 12, 13, 14, 15,255,255,255,255,255,255,255,255,255,  // 6
			255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  // 7
			255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  // 8
			255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  // 9
			255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  // A
			255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  // B
			255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  // C
			255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  // D
			255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  // E
			255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255   // F
		};

		// Upper case conversion
		template<int Dummy>
		const unsigned char lookup_tables<Dummy>::lookup_upcase[256] = 
		{
			// 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  A   B   C   D   E   F
			0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15,   // 0
			16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,   // 1
			32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,   // 2
			48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,   // 3
			64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,   // 4
			80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,   // 5
			96, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,   // 6
			80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 123,124,125,126,127,  // 7
			128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,  // 8
			144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,  // 9
			160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,  // A
			176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,  // B
			192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,  // C
			208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,  // D
			224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,  // E
			240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255   // F
		};
	}
	//! \endcond


}



#endif
