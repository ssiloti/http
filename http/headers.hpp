//
// headers.hpp
// ~~~~~~~~~
//
// Copyright (c) 2010 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_HEADERS_HPP
#define HTTP_HEADERS_HPP

#include <http/media_type.hpp>
#include <http/comment.hpp>

#include <uri/basic_uri.hpp>

#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <boost/fusion/container/map.hpp>
#include <boost/fusion/algorithm/transformation/join.hpp>
#include <boost/fusion/support/pair.hpp>
#include <boost/mpl/string.hpp>

#include <boost/variant.hpp>
#include <boost/optional.hpp>
#include <boost/cstdint.hpp>

#include <string>
#include <map>
#include <vector>

namespace http {
namespace headers {

struct host_type
{
    std::string host;
    boost::optional<boost::uint16_t> port;
};

struct entity_tag_type
{
    bool weak;
    std::string tag;
};

struct product_type
{
    void clear()
    {
        product.clear();
        version.clear();
    }

    std::string product, version;
};

struct via_intermediary
{
    struct protocol_t { std::string name, version; } received_protocol;
    host_type received_by;
    http::comment comment;
};

} }

BOOST_FUSION_ADAPT_STRUCT(
    http::headers::host_type,
    (std::string, host)
    (boost::optional<boost::uint16_t>, port)
)

BOOST_FUSION_ADAPT_STRUCT(
    http::headers::product_type,
    (std::string, product)
    (std::string, version)
)

BOOST_FUSION_ADAPT_STRUCT(
    http::headers::via_intermediary::protocol_t,
    (std::string, name)
    (std::string, version)
)

BOOST_FUSION_ADAPT_STRUCT(
    http::headers::via_intermediary,
    (http::headers::via_intermediary::protocol_t, received_protocol)
    (http::headers::host_type, received_by)
    (http::comment, comment)
)

namespace http {
namespace headers {

struct asterisk {};

template <typename Header>
inline void clear_header(Header& header)
{
    header.second.clear();
}

template <typename Name>
inline void clear_header(boost::fusion::pair<Name, boost::posix_time::ptime>& header)
{
    header.second = boost::posix_time::not_a_date_time;
}

// General headers

typedef boost::fusion::pair<boost::mpl::string<'cach', 'e-co', 'ntr', 'ol'>,
        std::map<std::string, std::string> > cache_control;

typedef boost::fusion::pair<boost::mpl::string<'conn', 'ecti', 'on'>,
        std::vector<std::string> > connection;

typedef boost::fusion::pair<boost::mpl::string<'date'>,
        boost::posix_time::ptime> date;

typedef boost::fusion::pair<boost::mpl::string<'prag', 'ma'>,
        std::map<std::string, std::string> > pragma;

typedef boost::fusion::pair<boost::mpl::string<'trai', 'ler'>,
        std::vector<std::string> > trailer;

typedef boost::fusion::pair<boost::mpl::string<'tran', 'sfer', '-enc', 'odin', 'g'>,
        std::map<std::string, std::map<std::string, std::string> > > transfer_encoding;

typedef boost::fusion::pair<boost::mpl::string<'upgr' , 'ade'>,
        std::vector<product_type> > upgrade;

typedef boost::fusion::pair<boost::mpl::string<'via'>,
        std::vector<via_intermediary> > via;

struct warning_value
{
    boost::uint16_t code;
    struct agent_t { std::string host; boost::optional<boost::uint16_t> port; } agent;
    std::string text;
    boost::optional<boost::posix_time::ptime> date;
};
typedef boost::fusion::pair<boost::mpl::string<'warn', 'ing'>,
        std::vector<warning_value> > warning;

typedef boost::fusion::pair<boost::mpl::string<'mime', '-ver', 'sion'>,
        std::pair<unsigned int, unsigned int> > mime_version;

// Request Header Fields

struct accept_value
{
    http::media_type media_range;
    struct params_t {
        float qvalue;
        std::map<std::string, std::string> ext;
    };
    boost::optional<params_t> params;
};
typedef boost::fusion::pair<boost::mpl::string<'acce', 'pt'>,
        std::vector<accept_value> > accept;

struct charset_t { std::string charset; boost::optional<float> qvalue; };
typedef boost::fusion::pair<boost::mpl::string<'acce', 'pt-c', 'hars', 'et'>,
        std::vector<charset_t> > accept_charset;

struct coding_t { std::string coding; boost::optional<float> qvalue; };
typedef boost::fusion::pair<boost::mpl::string<'acce', 'pt-e', 'ncod', 'ing'>,
        std::vector<coding_t> > accept_encoding;

struct language_t { std::string range; boost::optional<float> qvalue; };
typedef boost::fusion::pair<boost::mpl::string<'acce', 'pt-l', 'angu', 'age'>,
        std::vector<language_t> > accept_language;

struct credentials_type
{
    std::string scheme;
    boost::optional<boost::variant<std::string, std::map<std::string, std::string> > > params;
};
typedef boost::fusion::pair<boost::mpl::string<'auth', 'oriz', 'atio', 'n'>,
        credentials_type> authorization;

typedef boost::fusion::pair<boost::mpl::string<'expe', 'ct'>,
        std::vector<std::map<std::string, std::vector<std::string> > > > expect;

// TODO: This should use an email address library
typedef boost::fusion::pair<boost::mpl::string<'from'>,
        std::string> from;

typedef boost::fusion::pair<boost::mpl::string<'host'>,
        host_type> host;

typedef boost::fusion::pair<boost::mpl::string<'if-m', 'atch'>,
        boost::variant<asterisk, std::vector<entity_tag_type> > > if_match;

typedef boost::fusion::pair<boost::mpl::string<'if-m', 'odif', 'ied-', 'sinc', 'e'>,
        boost::posix_time::ptime> if_modified_since;

typedef boost::fusion::pair<boost::mpl::string<'if-n', 'one-', 'matc', 'h'>,
        boost::variant<asterisk, std::vector<entity_tag_type> > > if_none_match;

typedef boost::fusion::pair<boost::mpl::string<'if-r', 'ange'>,
        boost::variant<entity_tag_type, boost::posix_time::ptime> > if_range;

typedef boost::fusion::pair<boost::mpl::string<'if-u', 'nmod', 'ifie', 'd-si', 'nce'>,
        boost::posix_time::ptime> if_unmodified_since;

typedef boost::fusion::pair<boost::mpl::string<'max-', 'forw', 'ards'>,
        unsigned int> max_forwards;

template<>
inline void clear_header(max_forwards& header)
{
    header.second = 0;
}

typedef boost::fusion::pair<boost::mpl::string<'prox', 'y-au', 'thor', 'izat', 'ion'>,
        credentials_type> proxy_authorization;

struct byte_range_pre { std::size_t first; boost::optional<std::size_t> last; };
struct byte_range_post { boost::optional<std::size_t> first; std::size_t last; };
typedef std::vector<boost::variant<
    byte_range_pre,
    byte_range_post
> > byte_range_type;

struct other_range_type
{
    std::string units;
    std::string range_set;
};

typedef boost::fusion::pair<boost::mpl::string<'rang' , 'e'>,
        boost::variant<byte_range_type, other_range_type> > range;

typedef boost::fusion::pair<boost::mpl::string<'refe', 'rer'>,
        uri::basic_uri<std::string> > referer;

typedef boost::fusion::pair<boost::mpl::string<'te'>,
        std::map<std::string, std::map<std::string, std::string> > > te;

struct user_agent_value
{
    void clear()
    {
        product.clear();
        details.clear();
    }

    product_type product;
    std::vector<boost::variant<product_type, http::comment> > details;
};
typedef boost::fusion::pair<boost::mpl::string<'user', '-age', 'nt'>,
        user_agent_value> user_agent;

// Response Header Fields

typedef boost::fusion::pair<boost::mpl::string<'acce', 'pt-r', 'ange', 's'>,
        std::vector<std::string> > accept_ranges;

typedef boost::fusion::pair<boost::mpl::string<'age'>,
        boost::posix_time::seconds> age;

template<>
inline void clear_header(age& header)
{
    header.second = boost::posix_time::seconds(boost::posix_time::not_a_date_time);
}

typedef boost::fusion::pair<boost::mpl::string<'allo', 'w'>,
        std::vector<std::string> > allow;

typedef boost::fusion::pair<boost::mpl::string<'etag'>,
        entity_tag_type> etag;

// TODO: Add URI reference type to the URI library
typedef boost::fusion::pair<boost::mpl::string<'loca', 'tion'>,
        uri::basic_uri<std::string> > location;

struct challenge_type
{
    std::string scheme;
    std::map<std::string, std::string> params;
};
typedef boost::fusion::pair<boost::mpl::string<'prox', 'y-au', 'then', 'tica', 'te'>,
        std::vector<challenge_type> > proxy_authenticate;

typedef boost::fusion::pair<boost::mpl::string<'retr', 'y-af', 'ter'>,
        boost::variant<boost::posix_time::ptime, boost::posix_time::seconds> > retry_after;

typedef boost::fusion::pair<boost::mpl::string<'serv', 'er'>,
        user_agent_value> server;

typedef boost::fusion::pair<boost::mpl::string<'vary'>,
        boost::variant<asterisk, std::vector<std::string> > > vary;

typedef boost::fusion::pair<boost::mpl::string<'www-', 'auth', 'enti', 'cate'>,
        std::vector<challenge_type> > www_authenticate;

// Payload Header Fields

typedef boost::fusion::pair<boost::mpl::string<'cont', 'ent-', 'leng', 'th'>,
        std::size_t> content_length;

template<>
inline void clear_header(content_length& header)
{
    header.second = 0;
}

typedef boost::fusion::pair<boost::mpl::string<'cont', 'ent-', 'md5'>,
        boost::uint8_t[16]> content_md5;

typedef boost::fusion::pair<boost::mpl::string<'cont', 'ent-', 'rang', 'e'>,
        boost::variant<byte_range_type, other_range_type> > content_range;

// Representation Header Fields

typedef boost::fusion::pair<boost::mpl::string<'cont', 'ent-', 'enco', 'ding'>,
        std::vector<std::string> > content_encoding;

// TODO: Add libaray to handle languages per RFC 5646
typedef boost::fusion::pair<boost::mpl::string<'cont', 'ent-', 'lang', 'uage'>,
        std::vector<std::string> > content_language;

// TODO: Handle partial URI form
typedef boost::fusion::pair<boost::mpl::string<'cont', 'ent-', 'loca', 'tion'>,
        uri::basic_uri<std::string> > content_location;

typedef boost::fusion::pair<boost::mpl::string<'cont', 'ent-', 'type'>,
        http::media_type> content_type;

typedef boost::fusion::pair<boost::mpl::string<'expi', 'res'>,
        boost::posix_time::ptime> expires;

typedef boost::fusion::pair<boost::mpl::string<'last', '-mod', 'ifie', 'd'>,
        boost::posix_time::ptime> last_modified;


typedef boost::fusion::map<content_length, content_type> map;

} } // namespace headers namespace http

#endif
