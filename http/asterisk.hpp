//
// asterisk.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2011 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef HTTP_ASTERISK_HPP
#define HTTP_ASTERISK_HPP

namespace http {

struct asterisk
{
    asterisk() : c('*') {}
    char c;
};

}

#endif
