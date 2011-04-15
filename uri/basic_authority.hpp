//
// basic_authority.hpp
// ~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2010 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef URI_BASIC_AUTHORITY_HPP
#define URI_BASIC_AUTHORITY_HPP

#include <boost/spirit/home/qi/domain.hpp>
#include <boost/spirit/home/karma/domain.hpp>
//#include <boost/spirit/home/support/attributes.hpp>

#include <boost/fusion/tuple/tuple_tie.hpp>

#include <boost/optional.hpp>
#include <boost/cstdint.hpp>

namespace uri {

template <typename String>
class basic_authority
{
public:
    typedef String string_type;

#if 0
    template <typename Iterator>
    static const boost::spirit::qi::rule<Iterator, basic_authority<string_type>()>& parse_rule();

    string_type userinfo() const
    {
        return userinfo_ ? *userinfo_ : string_type();
    }

    void userinfo(const string_type& ui)
    {
        userinfo_ = ui;
    }

    const string_type& host() const
    {
        return host_;
    }

    void host(const string_type& h)
    {
        host_ = h;
    }
#endif

    void clear()
    {
        
    }

    boost::optional<string_type> userinfo;
    string_type host;
    boost::optional<boost::uint16_t> port;
};

namespace detail {

template <typename String>
struct authority_tuple
{
    typedef boost::fusion::tuple<
        boost::optional<String>&,
        String&,
        boost::optional<boost::uint16_t>&
    > type;
};

template <typename String>
struct const_authority_tuple
{
    typedef boost::fusion::tuple<
        const boost::optional<String>&,
        const String&,
        const boost::optional<boost::uint16_t>&
    > type;
};

} // namespace detail

} // namespace uri

namespace boost { namespace spirit { namespace traits {

template <typename String>
struct transform_attribute<
    uri::basic_authority<String>,
    typename uri::detail::authority_tuple<String>::type,
    boost::spirit::qi::domain
>
{
    typedef typename uri::detail::authority_tuple<String>::type type;

    static type pre(uri::basic_authority<String>& exposed)
    {
        return boost::fusion::tie(
                exposed.userinfo,
                exposed.host,
                exposed.port
            );
    }

    static void post(uri::basic_authority<String>&, type const&) { }

    static void fail(uri::basic_authority<String>& val) { }
};

template <typename String>
struct transform_attribute<
    const uri::basic_authority<String>,
    typename uri::detail::const_authority_tuple<String>::type,
    boost::spirit::karma::domain
>
{
    typedef typename uri::detail::const_authority_tuple<String>::type type;

    static type pre(const uri::basic_authority<String>& exposed)
    {
        return type(
                exposed.userinfo,
                exposed.host,
                exposed.port
            );
    }
};

} } } // namespace traits namespace spirit namespace boost

#endif
