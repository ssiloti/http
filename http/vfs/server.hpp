//
// server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2011 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef HTTP_VFS_SERVER_HP
#define HTTP_VFS_SERVER_HP

#include <http/vfs/directory.hpp>

namespace http { namespace vfs {

template <typename Server>
class server : public Server, public directory<typename Server::context_type>
{
public:
    typedef typename Server::context_type context_type;

    server(boost::asio::io_service& io, boost::asio::ip::tcp::endpoint listen)
        : Server(io, listen)
    {
    }

    virtual void incoming_request(typename Server::context_type ctx)
    {
        const std::string& path
            (boost::fusion::get<0>(boost::get<boost::fusion::tuple<std::string, boost::optional<std::string> > >(ctx.request.target)));
        dispatch(ctx,
                 path,
                 path.begin(),
                 std::find(path.begin(), path.end(), '/')
        );
    }
};

} }


#endif
