//
// allow.hpp
// ~~~~~~~~~
//
// Copyright (c) 2010 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef HTTP_HEADERS_ALLOW_HPP
#define HTTP_HEADERS_ALLOW_HPP

#include <string>
#include <vector>

namespace http {
namespace headers {

class allow_value
{

private:
	std::vector<std::string> allowed_methods_;
};

}
}

#endif