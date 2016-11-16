#include "../../xtest/include/xtest.hpp"
#include "../include/parser.hpp"
#include "../include/builder.hpp"
#include <thread>
#include <iostream>
xtest_run;

XTEST_SUITE(xhttper)
{
	XUNIT_TEST(parse)
	{
		const char *buf =
			"GET http://eclick.baidu.com/a.js?tu=u2310667&jk=3f4ff730444a9cb7&word=http%3A%2F%2Fe.firefoxchina.cn%2F%3Fcachebust%3D20160321&if=3&aw=250&ah=108&pt=96500&it=96500&vt=96500&csp=1920,1040&bcl=250,120&pof=250,120&top=0&left=0&total=1&rdm=1479249557254 HTTP/1.1\r\n"
			"Host: eclick.baidu.com\r\n"
			"User-Agent: Mozilla/5.0 (Windows NT 10.0; WOW64; rv:50.0) Gecko/20100101 Firefox/50.0\r\n"
			"Accept: */*\r\n"
			"Accept-Language: zh-CN,zh;q=0.8,en-US;q=0.5,en;q=0.3\r\n"
			"Accept-Encoding: gzip, deflate\r\n"
			"Referer: http://fragment.firefoxchina.cn/html/main_baidu_cloud_250x108.html\r\n"
			"Cookie: BAIDUID=9DF780D9B96413B1421F2758E92D4DEB:FG=1\r\n"
			"Connection: keep-alive\r\n\r\nhello world";

		xhttper::parser per;
		per.append(buf, strlen(buf) - 100);
		xassert(!per.parse_req());
		per.append(buf + (strlen(buf) -100),100);
		xassert(per.parse_req());

		xassert(per.get_method() == "GET");
		xassert(per.get_path() == "http://eclick.baidu.com/a.js?tu=u2310667&jk=3f4ff730444a9cb7&word=http%3A%2F%2Fe.firefoxchina.cn%2F%3Fcachebust%3D20160321&if=3&aw=250&ah=108&pt=96500&it=96500&vt=96500&csp=1920,1040&bcl=250,120&pof=250,120&top=0&left=0&total=1&rdm=1479249557254");
		xassert(per.get_version() == "HTTP/1.1");

		xassert(per.get_header("Host") == "eclick.baidu.com");
		xassert(per.get_header("User-Agent") == "Mozilla/5.0 (Windows NT 10.0; WOW64; rv:50.0) Gecko/20100101 Firefox/50.0");
		xassert(per.get_header("Accept") == "*/*");
		xassert(per.get_header("Accept-Language") == "zh-CN,zh;q=0.8,en-US;q=0.5,en;q=0.3");
		xassert(per.get_header("Accept-Encoding") == "gzip, deflate");
		xassert(per.get_header("Referer") == "http://fragment.firefoxchina.cn/html/main_baidu_cloud_250x108.html");
		xassert(per.get_header("Cookie") == "BAIDUID=9DF780D9B96413B1421F2758E92D4DEB:FG=1");
		xassert(per.get_header("Connection") == "keep-alive");
		per.reset_status();
		xassert(per.get_string(strlen("hello world"))== "hello world");
	}
#if 0
	XUNIT_TEST(Benchmark)
	{
		const char *buf = "GET /cookies HTTP/1.1\r\nHost: 127.0.0.1:8090\r\nConnection: keep-alive\r\nCache-Control: max-age=0\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\nUser-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.17 (KHTML, like Gecko) Chrome/24.0.1312.56 Safari/537.17\r\nAccept-Encoding: gzip,deflate,sdch\r\nAccept-Language: en-US,en;q=0.8\r\nAccept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.3\r\nCookie: name=wookie\r\n\r\n";
		int64_t count = 0;
		std::thread counter([&] {
			auto last_count = count;
			auto len = strlen(buf);
			do
			{
				std::cout << 1.0 / ((count - last_count) / 100000.0) << std::endl;
				last_count = count;
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			} while (1);
		});

		xhttper::parser per;
		per.append(buf, strlen(buf));
		do
		{
			xassert(per.parse_req());
			per.reset_status();
			per.append(buf, strlen(buf));
			count++;
		} while (1);
	}
#endif
	XUNIT_TEST(parse_rsp)
	{
		const char *buf = 
			"HTTP/1.1 200 OK\r\n"
			"Server: Microsoft-IIS/5.0\r\n"
			"Date: Thu,08 Mar 200707:17:51 GMT\r\n"
			"Connection: Keep-Alive\r\n"
			"Content-Length: 23330\r\n"
			"Content-Type: text/html\r\n"
			"Expries: Thu,08 Mar 2007 07:16:51 GMT\r\n"
			"Set-Cookie: ASPSESSIONIDQAQBQQQB=BEJCDGKADEDJKLKKAJEOIMMH; path=/\r\n"
			"Cache-control: private\r\n\r\n";

		xhttper::parser per;
		per.append(buf, strlen(buf) - 100);
		xassert(!per.parse_rsp());
		per.append(buf + (strlen(buf) - 100), 100);
		xassert(per.parse_rsp());

		xassert(per.get_version() == "HTTP/1.1");
		xassert(per.get_status() == "200");
		xassert(per.get_status_str() == "OK");
		xassert(per.get_header("Server") == "Microsoft-IIS/5.0");
		xassert(per.get_header("Date") == "Thu,08 Mar 200707:17:51 GMT");
		xassert(per.get_header("Connection") == "Keep-Alive");
		xassert(per.get_header("Content-Length") == "23330");
		xassert(per.get_header("Content-Type") == "text/html");
		xassert(per.get_header("Expries") == "Thu,08 Mar 2007 07:16:51 GMT");
		xassert(per.get_header("Set-Cookie") == "ASPSESSIONIDQAQBQQQB=BEJCDGKADEDJKLKKAJEOIMMH; path=/");
		xassert(per.get_header("Cache-control") == "private");
	}
}
#include <sstream>
XTEST_SUITE(builder)
{
	const char *buf =
		"HTTP/1.1 200 OK\r\n"
		"Server: Microsoft-IIS/5.0\r\n"
		"Date: Thu,08 Mar 200707:17:51 GMT\r\n"
		"Connection: Keep-Alive\r\n"
		"Content-Length: 23330\r\n"
		"Content-Type: text/html\r\n"
		"Expries: Thu,08 Mar 2007 07:16:51 GMT\r\n"
		"Set-Cookie: ASPSESSIONIDQAQBQQQB=BEJCDGKADEDJKLKKAJEOIMMH; path=/\r\n"
		"Cache-control: private\r\n\r\n";

	XUNIT_TEST(build)
	{
		xassert(xhttper::builder().build() == "HTTP/1.1 200 OK\r\n\r\n");
		xhttper::builder ber;
		ber.append_header("Server", "Microsoft-IIS/5.0");
		ber.append_header("Date", "Thu,08 Mar 200707:17:51 GMT");
		ber.append_header("Connection", "Keep-Alive");
		ber.append_header("Content-Length", "23330");
		ber.append_header("Content-Type", "text/html");
		ber.append_header("Expries", "Thu,08 Mar 2007 07:16:51 GMT");
		ber.append_header("Set-Cookie","ASPSESSIONIDQAQBQQQB=BEJCDGKADEDJKLKKAJEOIMMH; path=/");
		ber.append_header("Cache-control", "private");

		auto data = ber.build();
	}
	XUNIT_TEST(encode_chunked)
	{
		xassert(xhttper::builder().encode_chunked(std::string('*', 42)) == "2a\r\n" + std::string('*', 42) + "\r\n")
	}
}