//
// directory.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2011 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef HTTP_VFS_DIRECTORY_HPP
#define HTTP_VFS_DIRECTORY_HPP

#include <http/vfs/node.hpp>
#include <map>
#include <string>
#include <algorithm>

namespace http { namespace vfs {

template <typename Context>
class directory : public node<Context>
{
    typedef std::map<std::string, node<Context>&> entries_type;
public:
    virtual void dispatch(Context ctx,
                          const std::string& path,
                          std::string::const_iterator segment_begin,
                          std::string::const_iterator segment_end)
    {
        std::string::const_iterator next_segment_begin = segment_end;
        ++next_segment_begin;
        std::string::const_iterator next_segment_end = std::find(next_segment_begin, path.end(), '/');
        entries_type::const_iterator entry = entries_.find(std::string(next_segment_begin, next_segment_end));
        if (entry != entries_.end())
        {
            entry->second.dispatch(ctx, path, next_segment_begin, next_segment_end);
        }
        else
        {
            // TODO: send a 404?
        }
    }

    void add_node(std::string name, node<Context>& node)
    {
        entries_.insert(entries_type::value_type(name, node));
    }

private:
    entries_type entries_;
};

} } // namespace vfs namespace http

#endif
