#ifndef URI_DETAIL_PARSE_URI_HPP
#define URI_DETAIL_PARSE_URI_HPP

// Copyright 2009 Dean Michael Berris, Jeroen Habraken.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/qi_attr.hpp>
#include <boost/spirit/include/qi_core.hpp>
#include <boost/spirit/include/qi_eps.hpp>
#include <boost/spirit/include/qi_grammar.hpp>
#include <boost/spirit/include/qi_omit.hpp>
#include <boost/spirit/include/qi_raw.hpp>
#include <boost/spirit/include/qi_rule.hpp>
#include <boost/spirit/include/qi_sequence.hpp>
#include <boost/spirit/include/version.hpp>

#include <uri/detail/uri_parts.hpp>

namespace boost { namespace spirit { namespace traits {

template <typename String>
struct transform_attribute<
    uri::detail::uri_parts<String>,
    typename uri::detail::uri_parts_tuple<String>::type
#if SPIRIT_VERSION > 0x2030
	, boost::spirit::qi::domain
#endif
    >
{
    typedef typename uri::detail::uri_parts_tuple<String>::type type;

    static type pre(uri::detail::uri_parts<String>& parts) {
        typedef typename String string_type;

        boost::fusion::tuple<
            boost::optional<string_type>&,
            boost::optional<string_type>&,
            boost::optional<boost::uint16_t>&,
            string_type&
                > hier_part =
                boost::fusion::tie(
                        parts.user_info,
                        parts.host,
                        parts.port,
                        parts.path
                    );

        return boost::fusion::tie(
                parts.scheme,
                hier_part,
                parts.query,
                parts.fragment
            );
    }

    static void post(uri::detail::uri_parts<String>&, type const&) { }

#if SPIRIT_VERSION >= 0x2030
    static void fail(uri::detail::uri_parts<String>& val) { }
#endif
};

#if SPIRIT_VERSION < 0x2030
template <typename Exposed, typename Transformed>
struct transform_attribute<
    optional<Exposed>,
    Transformed,
    typename disable_if<is_same<optional<Exposed>, Transformed> >::type
    >
{
    typedef Transformed& type;
        
    static Transformed& pre(optional<Exposed> & val) { 
        if (!val)
            val = Transformed();
        return boost::get<Transformed>(val); 
    }
    
    static void post(optional<Exposed> &, Transformed const &) { }
};
#endif

} // namespace traits

} // namespace spirit

} // namespace boost

namespace uri {

namespace detail {

template <typename Iterator, class String>
struct uri_grammar : boost::spirit::qi::grammar<Iterator, uri_parts<String>()>
{
	typedef typename String string_type;

    uri_grammar() : uri_grammar::base_type(start, "uri")
	{
		const basic_parse_rules<iterator, string_type>& basic_rules
			= basic_parse_rules<iterator, string_type>::get();

        uri %= 
                basic_rules.scheme >> ':'
            >>  basic_rules.hier_part
            >>  -('?' >> basic_rules.query)
            >>  -('#' >> basic_rules.fragment);

        start %= uri.alias();
    }

    // start rule of grammar
    boost::spirit::qi::rule<Iterator, uri_parts<string_type>()> start;

    // actual uri parser
    boost::spirit::qi::rule<Iterator, typename uri_parts_tuple<string_type>::type()> uri;
};

template <class Range, class String>
inline bool parse(Range& range, uri_parts<String>& parts) {
    typedef typename range_iterator<Range>::type iterator;

	// Qualified boost::begin and boost::end because MSVC complains
	// of ambiguity on call to begin(range) and end(rand).
    iterator start_ = boost::begin(range);
    iterator end_ = boost::end(range);

    uri_grammar<iterator, String> grammar;

    bool ok = qi::parse(start_, end_, grammar, parts);

    return ok && start_ == end_;
}

} // namespace detail

} // namespace uri

#endif
