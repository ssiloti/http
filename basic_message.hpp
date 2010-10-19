//
// message.hpp
// ~~~~~~~~~~~
//
// Copyright (c) 2010 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef HTTP_MESSAGE_HPP
#define HTTP_MESSAGE_HPP

#include <http/detail/apply_to_known_header.hpp>
#include <http/detail/serialize_known_header.hpp>
#include <http/detail/parse_known_header.hpp>
#include <http/detail/reset_known_header.hpp>
#include <http/version_number.hpp>

#include <boost/fusion/container/map.hpp>
#include <boost/fusion/sequence/intrinsic/at_key.hpp>

#include <map>
#include <vector>

namespace http {

template <typename Headers, typename Body>
class basic_message
{
public:
	typedef Headers known_headers_type;
	typedef Body entity_body_type;

	version_number version() const   { return version_; }
	void version(version_number ver) { version_ = ver; }

	template <typename Header>
	typename Header::second_type& header()
	{
		return boost::fusion::at_key<typename Header::first_type>(known_headers_);
	}

	template <typename Header>
	const typename Header::second_type& header() const
	{
		return boost::fusion::at_key<typename Header::first_type>(known_headers_);
	}

	std::vector<std::string> header(const std::string& name) const
	{
		std::vector<std::string> value(1);
		detail::apply_to_known_header(
			known_headers_,
			name,
			detail::serialize_known_header<std::back_insert_iterator<std::string> >(
				std::back_inserter(value[0])
			)
		);

		if (value[0].empty()) {
			unknown_headers_type::const_iterator unknown_header(
				unknown_headers_.find(name)
			);
			if (unknown_header != unknown_headers_.end())
				value = unknown_header->second;
		}

		return value;
	}

	void header(const std::string& name, const std::string& value)
	{
		if (!detail::apply_to_known_header(
				known_headers_,
				name,
				detail::parse_known_header<std::string::const_iterator>(
					value.begin(),
					value.end()
					)
				)
			) {
			unknown_headers_type::iterator header(
				unknown_headers_.insert(
					std::make_pair(name, unknown_headers_type::value_type())
					).first
			);
			header->second.push_back(value);
		}
	}

	void remove_header(std::string name)
	{
		detail::apply_to_known_header(
			known_headers_,
			name,
			detail::reset_known_header()
		);
		unknown_headers_.erase(name);
	}

private:
	typedef std::map<std::string, std::vector<std::string> > unknown_headers_type;

	version_number version_;
	known_headers_type known_headers_;
	unknown_headers_type unknown_headers_;
};

}

#endif
