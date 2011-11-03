//
// headers.hpp
// ~~~~~~~~~
//
// Copyright (c) 2010 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_PARSERS_HEADERS_HPP
#define HTTP_PARSERS_HEADERS_HPP

#include <http/headers.hpp>
#include <http/parsers/comment.hpp>
#include <http/parsers/date.hpp>
#include <http/parsers/media_type.hpp>
#include <http/parsers/entity_tag.hpp>
#include <http/parsers/basic_rules.hpp>
#include <uri/parsers/authority.hpp>
#include <uri/parsers/characters.hpp>

#include <boost/spirit/include/qi_parse_auto.hpp>
#include <boost/spirit/home/qi/numeric.hpp>
#include <boost/spirit/home/qi/operator.hpp>
#include <boost/spirit/home/qi/directive/repeat.hpp>
#include <boost/spirit/home/qi/directive/as.hpp>
#include <boost/spirit/home/qi/auxiliary/attr.hpp>
#include <boost/spirit/home/qi/auxiliary/attr_cast.hpp>

namespace http { namespace parsers {

template <typename Header, typename InputIterator>
bool parse_header(Header& header, InputIterator begin, InputIterator end)
{
    header.second = std::string(begin, end);
    return true;
}

template <typename InputIterator>
bool parse_header(headers::cache_control& header, InputIterator begin, InputIterator end)
{
    basic_rules<InputIterator> b;
    return boost::spirit::qi::phrase_parse(begin, end, (b.token >> -('=' >> (b.token | b.quoted_string))) % ',', b.skipper, header.second) && begin == end;
}

template <typename InputIterator>
bool parse_header(headers::connection& header, InputIterator begin, InputIterator end)
{
    basic_rules<InputIterator> b;
    return boost::spirit::qi::phrase_parse(begin, end, b.token % ',', b.skipper, header.second) && begin == end;
}

template <typename InputIterator>
bool parse_header(headers::date& header, InputIterator begin, InputIterator end)
{
    basic_rules<InputIterator> b;
    return boost::spirit::qi::phrase_parse(begin, end, date_grammar<InputIterator>(), b.skipper, header.second) && begin == end;
}

template <typename InputIterator>
bool parse_header(headers::pragma& header, InputIterator begin, InputIterator end)
{
    basic_rules<InputIterator> b;
    return boost::spirit::qi::phrase_parse(begin, end, (b.token >> -('=' >> (b.token | b.quoted_string))) % ',', b.skipper, header.second) && begin == end;
}

template <typename InputIterator>
bool parse_header(headers::trailer& header, InputIterator begin, InputIterator end)
{
    basic_rules<InputIterator> b;
    return boost::spirit::qi::phrase_parse(begin, end, b.token % ',', b.skipper, header.second) && begin == end;
}

template <typename InputIterator>
bool parse_header(headers::transfer_encoding& header, InputIterator begin, InputIterator end)
{
    basic_rules<InputIterator> b;
    return boost::spirit::qi::phrase_parse(begin, end, (b.token >> *(';' >> b.token >> '=' >> b.word)) % ',', b.skipper, header.second) && begin == end;
}

template <typename InputIterator>
bool parse_header(headers::upgrade& header, InputIterator begin, InputIterator end)
{
    basic_rules<InputIterator> b;
    return boost::spirit::qi::phrase_parse(begin, end, (b.token >> -('/' >> b.token)) % ',', b.skipper, header.second) && begin == end;
}

template <typename InputIterator>
bool parse_header(headers::via& header, InputIterator begin, InputIterator end)
{
    using namespace boost::spirit::qi;

    basic_rules<InputIterator> b;
    uri::parsers::authority<InputIterator, std::string> a;
    parsers::comment<InputIterator> c;

    rule<InputIterator, headers::via_intermediary::protocol_t()> rp = -(as_string[b.token >> '/']) >> b.token;
    rule<InputIterator, headers::host_type()> rb = (a.host >> -(':' >> ushort_)) | (b.token >> attr(boost::optional<boost::uint16_t>()));
    return phrase_parse(begin, end, (rp >> rb >> -c) % ',', b.skipper, header.second) && begin == end;
}

template <typename InputIterator>
bool parse_header(headers::warning& header, InputIterator begin, InputIterator end)
{
    using namespace boost::spirit::qi;

    basic_rules<InputIterator> b;
    uri::parsers::authority<InputIterator, std::string> a;

    rule<InputIterator, boost::uint16_t()> code = uint_parser<unsigned, 10, 3, 3>();
    rule<InputIterator, headers::host_type()> agent = a.host >> -(':' >> ushort_);
    rule<InputIterator, std::string()> text = b.quoted_string;
    // For some reason parsing the date fails when it's in its own rule *shrug*
//    rule<InputIterator, boost::posix_time::ptime()> date = lit('"') >> date_grammar<InputIterator>() >> lit('"');
    return phrase_parse(begin, end, (code >> agent >> text >> -(lit('"') >> date_grammar<InputIterator>() >> lit('"'))) % ',', b.skipper, header.second) && begin == end;
}

template <typename InputIterator>
bool parse_header(headers::mime_version& header, InputIterator begin, InputIterator end)
{
    return boost::spirit::qi::phrase_parse(begin, end, uint_parser<unsigned, 10, 1, 1>() >> '.' >> uint_parser<unsigned, 10, 1, 1>(), b.skipper, header.second) && begin == end;
}

template <typename InputIterator>
bool parse_header(headers::accept& header, InputIterator begin, InputIterator end)
{
    using namespace boost::spirit::qi;
    basic_rules<InputIterator> b;

    rule<InputIterator, boost::optional<headers::accept_value::params_t>()> accept_params = -(';' >> b.ows >> "q=" >> real_parser<float, ureal_policies<float> >() >> *(';' >> b.token >> '=' >> b.word));
    return phrase_parse(begin, end, (media_type<InputIterator>() >> accept_params) % ',', b.skipper, header.second) && begin == end;
}

template <typename InputIterator>
bool parse_header(headers::accept_charset& header, InputIterator begin, InputIterator end)
{
    using namespace boost::spirit::qi;
    basic_rules<InputIterator> b;
    return phrase_parse(begin, end, (b.token >> -(';' >> b.ows >> "q=" >> real_parser<float, ureal_policies<float> >())) % ',', b.skipper, header.second) && begin == end;
}

// TODO: Eliminate code duplication
template <typename InputIterator>
bool parse_header(headers::accept_encoding& header, InputIterator begin, InputIterator end)
{
    using namespace boost::spirit::qi;
    basic_rules<InputIterator> b;
    return phrase_parse(begin, end, (b.token >> -(';' >> b.ows >> "q=" >> real_parser<float, ureal_policies<float> >())) % ',', b.skipper, header.second) && begin == end;

}

template <typename InputIterator>
bool parse_header(headers::accept_language& header, InputIterator begin, InputIterator end)
{
    using namespace boost::spirit::qi;
    basic_rules<InputIterator> b;
    rule<InputIterator, std::string()> language_range = char_('*') | (repeat(1, 8)[b.alpha] >> *(char_('-') >> repeat(1, 8)[b.alpha | b.digit]));
    return phrase_parse(begin, end, (language_range >> -(';' >> b.ows >> "q=" >> real_parser<float, ureal_policies<float> >())) % ',', b.skipper, header.second) && begin == end;
}

template <typename InputIterator>
bool parse_header(headers::authorization& header, InputIterator begin, InputIterator end)
{
    using namespace boost::spirit::qi;
    basic_rules<InputIterator> b;
    rule<InputIterator, std::map<std::string, std::string>()> paramv = (b.token >> (b.token | b.quoted_string)) % ',';
    rule<InputIterator, headers::credentials_t::params_t()> auth_param = -(as_string[b.token] | b.quoted_string | paramv);
    rule<InputIterator, headers::credentials_t::params_t()> basic_credentials = -(as_string[+char_("A-Za-z0-9+/=")] | paramv);
    return phrase_parse(begin, end, (string("Basic") >> basic_credentials) | (b.token >> auth_param), b.skipper, header.second) && begin == end;
}

template <typename InputIterator>
bool parse_header(headers::expect& header, InputIterator begin, InputIterator end)
{
    basic_rules<InputIterator> b;
    return boost::spirit::qi::phrase_parse(begin, end, (b.token >> -('=' >> (b.token | b.quoted_string) >> *(';' >> b.token >> '=' >> (b.token | b.quoted_string)))) % ',', b.skipper, header.second) && begin == end;
}

template <typename InputIterator>
bool parse_header(headers::from& header, InputIterator begin, InputIterator end)
{
    using namespace boost::spirit::qi;
    basic_rules<InputIterator> b;
    return phrase_parse(begin, end, string, b.skipper, header.second) && begin == end;
}

template <typename InputIterator>
bool parse_header(headers::host& header, InputIterator begin, InputIterator end)
{
    basic_rules<InputIterator> b;
    uri::parsers::authority<InputIterator, std::string> a;
    return boost::spirit::qi::phrase_parse(begin, end, a.host >> -(':' >> boost::spirit::qi::ushort_), b.skipper, header.second) && begin == end;
}

template <typename InputIterator>
bool parse_header(headers::if_match& header, InputIterator begin, InputIterator end)
{
    using namespace boost::spirit::qi;
    basic_rules<InputIterator> b;
    return phrase_parse(begin, end, (lit('*') >> attr(asterisk())) | (entity_tag<InputIterator>() % ','), b.skipper, header.second) && begin == end;
}

template <typename InputIterator>
bool parse_header(headers::allow& header, InputIterator begin, InputIterator end)
{
    basic_rules<InputIterator> b;
    return boost::spirit::qi::phrase_parse(begin, end, b.token % ',', b.skipper, header.second) && begin == end;
}

template <typename InputIterator>
bool parse_header(headers::content_length& header, InputIterator begin, InputIterator end)
{
    return boost::spirit::qi::parse(begin, end, header.second) && begin == end;
}

template <typename InputIterator>
bool parse_header(headers::content_type& header, InputIterator begin, InputIterator end)
{
    basic_rules<InputIterator> b;
    return boost::spirit::qi::phrase_parse(begin, end, media_type<InputIterator>(), b.skipper, header.second) && begin == end;
}

/*
template <typename InputIterator>
bool parse_header(headers::& header, InputIterator begin, InputIterator end)
{
}
*/

} } // namespace parsers namespace http

#endif
