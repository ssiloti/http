//
// status_code.hpp
// ~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2011 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef HTTP_STATUS_CODE_HPP
#define HTTP_STATUS_CODE_HPP

namespace http {

enum status_code {
    status_continue             = 100,
    status_ok                   = 200,
    status_multiple_choices     = 300,
    status_bad_request          = 400,
    status_internal_server_error= 500,
};

#if 0
const struct
{
    status_code status;
    const char* reason;
} reason_map[] =
{{status_continue, "Continue"}};
#endif

} // namespace http

#endif

