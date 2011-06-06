//
// static_file.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2011 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef HTTP_VFS_STATIC_FILE_HPP
#define HTTP_VFS_STATIC_FILE_HPP

#include <http/vfs/node.hpp>
#include <http/basic_response.hpp>
#include <http/headers.hpp>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>

namespace http { namespace vfs {

template <typename Context>
class static_file : public node<Context>
{
public:
    static_file(std::string name, media_type type)
    {
        response_.headers.at<headers::content_type>() = type;
        boost::interprocess::file_mapping mapping(name, boost::interprocess::read_only);
        response_.body = boost::make_shared<boost::interprocess::mapped_region>(mapping, boost::interprocess::read_only);
    }

    virtual void dispatch(context_type ctx,
                          const std::string& path,
                          std::string::const_iterator segment_begin,
                          std::string::const_iterator segment_end)
    {
        ctx.write_response(response_);
    }

private:
    basic_response<headers::map, boost::shared_ptr<boost::interprocess::mapped_region> > response_;
};

}

template <typename OutputIterator>
boost::fusion::map<headers::content_length> generate_body(OutputIterator sink, const boost::interprocess::mapped_region& region)
{
    sink = boost::asio::buffer(region.get_address(), region.get_size());
    return boost::fusion::map<headers::content_length>(headers::content_length(region.get_size()));;
}

}

#endif
