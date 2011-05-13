//
// async_server.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2011 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef HTTP_ASYNC_SERVER_HPP
#define HTTP_ASYNC_SERVER_HPP

#include <http/basic_server_impl.hpp>
#include <http/basic_async_server.hpp>

#include <string>

namespace http {

typedef basic_async_server<headers::map, std::string> async_string_server;

}

#endif
