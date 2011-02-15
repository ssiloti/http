//
// basic_authority.hpp
// ~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2010 Steven Siloti (ssiloti@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/spirit/home/qi/domain.hpp>
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

} // namespace detail

} // namespace uri

namespace boost { namespace spirit { namespace traits {
//#if 0
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
//#endif
#if 0
template <typename String>
struct transform_attribute<
    uri::basic_authority<String>,
    String,
    boost::spirit::qi::domain
>
{
    typedef String& type;

    static type pre(uri::basic_authority<String>& exposed)
    {
        return exposed.host;
    }

    static void post(uri::basic_authority<String>&, const String&) { }

    static void fail(uri::basic_authority<String>& val) { }
};
#endif
} } } // namespace traits namespace spirit namespace boost

#if 0
namespace uri {

template <typename String>
template <typename Iterator>
const boost::spirit::qi::rule<Iterator, basic_authority<String>()>& basic_authority<String>::parse_rule()
{
    using namespace boost::spirit::qi;

    const basic_parse_rules<Iterator, String>& basic_rules
            = basic_parse_rules<iterator, String>::get();

    static rule<Iterator, basic_authority<String>()> authority;
    authority %= -(basic_rules.user_info >> '@') >> basic_rules.host >> -(':' >> ushort_);
    return authority;
}

} // namespace uri
#endif
