#ifndef URI_DETAIL_URI_PARTS_HPP
#define URI_DETAIL_URI_PARTS_HPP

// Copyright 2009 Dean Michael Berris, Jeroen Habraken.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/cstdint.hpp>
#include <boost/fusion/tuple.hpp>
#include <boost/optional.hpp>

namespace uri {

namespace detail {

template <class String>
struct uri_parts
{
    typedef typename String string_type;

    string_type scheme;
    boost::optional<string_type> user_info;
    boost::optional<string_type> host;
    boost::optional<boost::uint16_t> port;
    string_type path;
    boost::optional<string_type> query;
    boost::optional<string_type> fragment;
};

template <class String>
struct uri_parts_tuple
{
    typedef typename String string_type;

    typedef typename boost::fusion::tuple<
        string_type&,
        boost::fusion::tuple<
            boost::optional<string_type>&,
            boost::optional<string_type>&,
            boost::optional<boost::uint16_t>&,
            string_type&
            >,
        boost::optional<string_type>&,
        boost::optional<string_type>&
        > type;
};

template <class String>
inline void swap(uri_parts<String>& l, uri_parts<String>& r)
{
    using std::swap;

    swap(l.scheme, r.scheme);
    swap(l.user_info, r.user_info);
    swap(l.host, r.host);
    swap(l.port, r.port);
    swap(l.path, r.path);
    swap(l.query, r.query);
    swap(l.fragment, r.fragment);
}

template <class String>
inline 
bool operator==(uri_parts<String> const & l, uri_parts<String> const & r)
{
    return (l.scheme == r.scheme) &&
           (l.user_info == r.user_info) &&
           (l.host == r.host) &&
           (l.port == r.port) &&
           (l.path == r.path) &&
           (l.query == r.query) &&
           (l.fragment == r.fragment);
}

template <class Tag>
inline
bool operator!=(uri_parts<Tag> const & l, uri_parts<Tag> const & r)
{
    return !(l == r);
}

} // namespace detail

} // namespace uri

#endif
