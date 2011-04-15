//
// node.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2011 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef HTTP_VFS_NODE_HPP
#define HTTP_VFS_NODE_HPP

#include <string>

namespace http { namespace vfs {

template <typename Context>
class node
{
public:
    typedef Context context_type;
    virtual void dispatch(context_type ctx,
                          const std::string& path,
                          std::string::const_iterator segment_begin,
                          std::string::const_iterator segment_end) = 0;
};

} } // namespace vfs namespace http

#endif
