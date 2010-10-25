//
// headers_map.hpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2010 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef HTTP_HEADERS_MAP_HPP
#define HTTP_HEADERS_MAP_HPP

#include <boost/mpl/begin_end.hpp>
#include <boost/mpl/next.hpp>
#include <boost/mpl/equal_to.hpp>
#include <boost/mpl/deref.hpp>
#include <boost/mpl/push_front.hpp>
#include <boost/mpl/vector/vector0.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/mpl/string.hpp>

#include <boost/variant/variant.hpp>
#include <boost/variant/apply_visitor.hpp>

#include <boost/type_traits/is_same.hpp>

#include <string>
#include <map>
#include <utility>
#include <algorithm>

namespace http {

template <typename Known>
class headers_map
{
public:
	typedef Known known_headers_type;

private:
	typedef std::string unkown_header_type;

	typedef typename boost::mpl::push_front<
		typename boost::mpl::transform<
			known_headers_type,
			typename boost::mpl::second<boost::mpl::_1>,
			boost::mpl::back_inserter<boost::mpl::vector0<> >
			>::type,
		unkown_header_type
		>::type ref_types;
	typedef typename boost::make_variant_over<ref_types>::type variant_type;

public:
	class mapped_type
	{
		template <typename OutputIterator>
		struct serialize_visitor : public boost::static_visitor<>
		{
			serialize_visitor(OutputIterator s)
				: sink(s)
			{}

			template <typename Header>
			void operator()(const Header& header) const
			{
				header.serialize(sink);
			}

			template <>
			void operator()(const unkown_header_type& header) const
			{
				std::copy(header.begin(), header.end(), sink);
			}

			OutputIterator sink;
		};

		template <typename InputIterator>
		struct assign_visitor : public boost::static_visitor<>
		{
			typedef InputIterator iterator;

			assign_visitor(iterator begin, iterator end)
				: begin_(begin), end_(end)
			{}

			template <typename Header>
			void operator()(Header& header) const
			{
				header.clear();
				header.parse(begin_, end_);
			}

			template <>
			void operator()(unkown_header_type& header) const
			{
				header.clear();
				std::copy(begin_, end_, std::back_inserter(header));
			}

			iterator begin_, end_;
		};

		template <typename InputIterator>
		struct append_visitor : public boost::static_visitor<>
		{
			typedef InputIterator iterator;

			append_visitor(iterator begin, iterator end)
				: begin_(begin), end_(end)
			{}

			template <typename Header>
			void operator()(Header& header) const
			{
				header.parse(begin_, end_);
			}

			template <>
			void operator()(unkown_header_type& header) const
			{
				if (!header.empty())
					header.push_back(',');
				std::copy(begin_, end_, std::back_inserter(header));
			}

			iterator begin_, end_;
		};

		struct clear_visitor : public boost::static_visitor<>
		{
			template <typename Header>
			void operator()(Header& header) const
			{
				header.clear();
			}
		};

	public:
		mapped_type()
		{}

		template <typename Val>
		mapped_type(const Val& val)
			: var_(val)
		{}

		template <typename InputIterator>
		mapped_type(InputIterator begin, InputIterator end)
			: var_(std::string(begin, end))
		{}

		template <typename InputIterator>
		mapped_type& assign(InputIterator begin, InputIterator end)
		{
			boost::apply_visitor(assign_visitor<InputIterator>(begin, end), var_);
			return *this;
		}

		mapped_type& assign(const std::string& s)
		{
			return assign(s.begin(), s.end());
		}

		template <typename InputIterator>
		mapped_type& append(InputIterator begin, InputIterator end)
		{
			boost::apply_visitor(append_visitor<InputIterator>(begin, end), var_);
			return *this;
		}

		mapped_type& append(const std::string& s)
		{
			return append(s.begin(), s.end());
		}

		template <typename OutputIterator>
		void serialize(OutputIterator sink)
		{
			boost::apply_visitor(serialize_visitor<OutputIterator>(sink), var_);
		}

		void clear()
		{
			boost::apply_visitor(clear_visitor(), var_);
		}

		operator std::string() const
		{
			std::string value;
			boost::apply_visitor(serialize_visitor<std::back_insert_iterator<std::string> >(std::back_inserter(value)), var_);
			return value;
		}

		mapped_type& operator=(const std::string& s)
		{
			return assign(s);
		}

		mapped_type& operator+=(const std::string& s)
		{
			return append(s);
		}

	private:
		friend class headers_map;

		variant_type var_;
	};

private:
	typedef std::map<std::string, mapped_type> base_type;

	template <typename First, typename Last>
	variant_type variant_from_name(std::string, boost::mpl::true_)
	{
		return unkown_header_type();
	}

	template <typename First, typename Last>
	variant_type variant_from_name(std::string name, boost::mpl::false_)
	{
		using namespace boost;

		typedef typename mpl::deref<First>::type item_type;
		if (name == mpl::c_str<item_type::first>::value)
			return item_type::second();
		else
			return variant_from_name<typename mpl::next<First>::type, Last>(
				name,
				is_same<typename mpl::next<First>::type, Last>()
				);
	}

	variant_type variant_from_name(std::string name)
	{
		using namespace boost;

		return variant_from_name<
			mpl::begin<known_headers_type>::type,
			mpl::end<known_headers_type>::type
			>(
			name,
			is_same<typename mpl::begin<known_headers_type>::type,
				typename mpl::end<known_headers_type>::type>()
			);
	}

public:
	typedef typename base_type::key_type key_type;
	typedef typename base_type::value_type value_type;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef typename base_type::iterator iterator;
	typedef typename base_type::const_iterator const_iterator;

	const_iterator begin() const
	{
		return headers_.begin();
	}

	const_iterator end() const
	{
		return headers_.end();
	}

	std::pair<const_iterator, bool> insert(const value_type& value)
	{
		std::pair<iterator, bool> result = headers_.insert(value_type(value.first, variant_from_name(value.first)));
		if (result.second)
			result.first->second.assign(value.second);
		return std::make_pair(const_iterator(result.first), result.second);
	}

	const_iterator append(const value_type& value)
	{
		iterator result = headers_.insert(value_type(value.first, variant_from_name(value.first))).first;
		result->second.append(value.second);
		return result;
	}

	const_iterator find(const key_type& key) const
	{
		return headers_.find(key);
	}

	template <typename Header>
	typename Header::second& at()
	{
		return boost::get<typename Header::second>(
			headers_.insert(
				std::make_pair(
					boost::mpl::c_str<typename Header::first>::value,
					variant_type(Header::second())
					)
				).first->second.var_
			);
	}

	template <typename Header>
	typename const Header::second& at() const
	{
		return boost::get<typename Header::second>(
			headers_.insert(
				std::make_pair(
					boost::mpl::c_str<typename Header::first>::value,
					variant_type(Header::second())
					)
				).first->second.var_
			);
	}

private:
	base_type headers_;
};

}

#endif
