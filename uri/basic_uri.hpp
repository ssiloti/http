#ifndef URI_BASIC_URI_HPP
#define URI_BASIC_URI_HPP

// Copyright 2009 Dean Michael Berris, Jeroen Habraken.
// Copyright 2011 Steven Siloti.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <uri/basic_authority.hpp>

namespace uri {

class invalid_uri : public std::exception
{
};

template <class String>
struct basic_uri
{
    typedef String string_type;

    basic_uri() {}
    basic_uri(string_type const& uri);
    basic_uri(const typename string_type::value_type* uri);

#if 0
    basic_uri& operator=(basic_uri<string_type> other)
    {
        other.swap(*this);
        return *this; 
    }

    basic_uri& operator=(string_type const& uri)
    {
        return *this = basic_uri<string_type>(uri);
    }
#endif
#if 0
    void swap(basic_uri<string_type>& other)
    {
        using std::swap;

        swap(other.parts_, parts_);
    }


    bool operator==(basic_uri<string_type> const& other) const
    {
        return scheme == other.scheme
            && authority == other.authority
            && path == other.path
            && query == other.query
            && fragment == other.fragment;
    }
    #endif
    bool operator!=(basic_uri<string_type> const& other) const
    {
        return !(*this == other);
    }

    string_type scheme;
    boost::optional<basic_authority<string_type> > authority;
    string_type path;
    boost::optional<string_type> query;
    boost::optional<string_type> fragment;
};

namespace detail {

template <class String>
struct basic_uri_tuple
{
    typedef String string_type;

    typedef typename boost::fusion::tuple<
        string_type&,                   // scheme
        boost::fusion::tuple<
            boost::optional<basic_authority<string_type> >&,
            string_type&                // path
        >,                              // hier_part
        boost::optional<string_type>&,  // query
        boost::optional<string_type>&   // fragment
    > type;
};

template <class String>
struct const_basic_uri_tuple
{
    typedef String string_type;

    typedef typename boost::fusion::tuple<
        const string_type&,                   // scheme
        const boost::optional<basic_authority<string_type> >&,
        const string_type&,                // path
        const boost::optional<string_type>&,  // query
        const boost::optional<string_type>&   // fragment
    > type;
};

}

} // namespace uri

namespace boost { namespace spirit { namespace traits {

template <typename String>
struct transform_attribute<
    uri::basic_uri<String>,
    typename uri::detail::basic_uri_tuple<String>::type,
    boost::spirit::qi::domain
>
{
    typedef typename uri::detail::basic_uri_tuple<String>::type type;

    static type pre(uri::basic_uri<String>& exposed) {
        return type(
                exposed.scheme,
                boost::fusion::tie(
                    exposed.authority,
                    exposed.path
                ),
                exposed.query,
                exposed.fragment
            );
    }

    static void post(uri::basic_uri<String>&, type const&) { }
    static void fail(uri::basic_uri<String>& val) { }
};

template <typename String>
struct transform_attribute<
    const uri::basic_uri<String>,
    typename uri::detail::const_basic_uri_tuple<String>::type,
    boost::spirit::karma::domain
>
{
    typedef typename uri::detail::const_basic_uri_tuple<String>::type type;

    static type pre(const uri::basic_uri<String>& exposed) {
        return type(
                exposed.scheme,
                exposed.authority,
                exposed.path,
                exposed.query,
                exposed.fragment
            );
    }
};

} // namespace traits

} // namespace spirit

} // namespace boost

#endif
