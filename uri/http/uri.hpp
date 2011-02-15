#ifndef URI_HTTP_URI_HPP_
#define URI_HTTP_URI_HPP_

// Copyright 2009 Dean Michael Berris, Jeroen Habraken.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/cstdint.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <uri/basic_uri.hpp>

namespace uri {

template <typename String>
struct http_uri : public uri_base<String>
{
    typedef String string_type;
    using uri_base<string_type>::operator=;
    using uri_base<string_type>::swap;

    friend struct boost::spirit::traits::transform_attribute<
        http_uri<String>,
        typename uri::detail::uri_parts_tuple<String>::type
    #if SPIRIT_VERSION > 0x2030
        , boost::spirit::qi::domain
    #endif
        >;
#if 0
    http_uri() : uri_base<string_type>() {}
    http_uri(uri_base<string_type>::string_type const& uri)
        : uri_base<string_type>(uri)
    {
        if ((scheme().size() < 4) || (scheme().size() > 5))
            throw invalid_uri();

        if (scheme().size() == 4) {
            if (not boost::iequals(scheme(), "http"))
                throw invalid_uri();
        } else { // size is 5
            if (not boost::iequals(scheme(), "https"))
                throw invalid_uri();
        }

        if ((not host()) || host()->empty())
            throw invalid_uri();
    }
#endif
    boost::uint16_t port() const
    {
        return parts_.port ? *(parts_.port) : 
            (boost::iequals(parts_.scheme, string_type("https")) ? 443u : 80u);
    }

    string_type path() const
    {
        return (parts_.path == "") ? string_type("/") : parts_.path;
    }
};

} // namespace uri

namespace boost { namespace spirit { namespace traits {

template <typename String>
struct transform_attribute<
    uri::http_uri<String>,
    typename uri::detail::uri_parts_tuple<String>::type
#if SPIRIT_VERSION > 0x2030
	, boost::spirit::qi::domain
#endif
    >
{
    typedef typename uri::detail::uri_parts_tuple<String>::type type;

    static type pre(uri::http_uri<String>& exposed) {
        typedef typename String string_type;

        boost::fusion::tuple<
            boost::optional<string_type>&,
            boost::optional<string_type>&,
            boost::optional<boost::uint16_t>&,
            string_type&
                > hier_part =
                boost::fusion::tie(
                        exposed.parts_.user_info,
                        exposed.parts_.host,
                        exposed.parts_.port,
                        exposed.parts_.path
                    );

        return boost::fusion::tie(
                exposed.parts_.scheme,
                hier_part,
                exposed.parts_.query,
                exposed.parts_.fragment
            );
    }

    static void post(uri::http_uri<String>&, type const&) { }

#if SPIRIT_VERSION >= 0x2030
    static void fail(uri::http_uri<String>& val) { }
#endif
};

} // namespace traits

} // namespace spirit

} // namespace boost

namespace uri {

template <typename Iterator, class String>
struct http_uri_grammar : qi::grammar<Iterator, http_uri<String>()>
{
    typedef typename String string_type;

    http_uri_grammar() : http_uri_grammar::base_type(start, "uri")
    {
        const basic_parse_rules<Iterator, string_type>& basic_rules
            = basic_parse_rules<Iterator, string_type>::get();

        uri %= 
                basic_rules.scheme >> ':'
            >>  basic_rules.hier_part
            >>  -('?' >> basic_rules.query)
            >>  -('#' >> basic_rules.fragment);

        start %= uri.alias();
    }

    // start rule of grammar
    boost::spirit::qi::rule<Iterator, http_uri<string_type>()> start;

    // actual uri parser
    boost::spirit::qi::rule<Iterator, typename detail::uri_parts_tuple<string_type>::type()> uri;
};

template <class Range, class String>
inline bool parse(Range& range, http_uri<String>& parts) {
    typedef typename boost::range_iterator<Range>::type iterator;

    // Qualified boost::begin and boost::end because MSVC complains
    // of ambiguity on call to begin(range) and end(rand).
    iterator start_ = boost::begin(range);
    iterator end_ = boost::end(range);

    http_uri_grammar<iterator, String> grammar;

    bool ok = qi::parse(start_, end_, grammar, parts);

    return ok && start_ == end_;
}

} // namespace uri

#endif
