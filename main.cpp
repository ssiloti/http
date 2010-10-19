#include <http/headers/content_length.hpp>
#include <http/basic_message.hpp>

#include <boost/fusion/container/map.hpp>
#include <boost/mpl/string.hpp>

#include <iostream>

int main()
{
	namespace f = boost::fusion;
	namespace m = boost::mpl;
	using namespace http::headers;

	typedef f::map<content_length> headers;

	http::basic_message<headers, void> msg;

	msg.header<content_length>() = 1234;
	std::cout << msg.header("content-length")[0];
}
