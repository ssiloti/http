//
// static_directory.hpp
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
//#include <boost/filesystem/path.hpp>

namespace http { namespace vfs {

template <typename Context>
class static_directory : public node<Context>
{
    typedef basic_response<headers::map, boost::shared_ptr<boost::interprocess::mapped_region> > response_type;
public:
    static_directory(std::string path, media_type type)
        : path_(path), type_(type)
    {
    }

    virtual void dispatch(context_type ctx,
                          const std::string& path,
                          std::string::const_iterator segment_begin,
                          std::string::const_iterator segment_end)
    {
        boost::shared_ptr<response_type> response(boost::make_shared<response_type>());
        response->headers.at<headers::content_type>() = type_;
        boost::interprocess::file_mapping mapping((path_ + std::string(segment_end, path.end())).c_str(), boost::interprocess::read_only);
        response->body = boost::make_shared<boost::interprocess::mapped_region>(mapping, boost::interprocess::read_only);
        ctx.write_response(response);
    }

private:
    media_type type_;
    std::string path_;
};

}

template <typename OutputIterator>
boost::fusion::map<headers::content_length> generate_body(OutputIterator sink, boost::shared_ptr<boost::interprocess::mapped_region> region)
{
    sink = boost::asio::buffer(region->get_address(), region->get_size());
    return boost::fusion::map<headers::content_length>(headers::content_length(region->get_size()));;
}

}

#endif
