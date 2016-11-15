#include "../../xtest/include/xtest.hpp"
#include "../include/parser.hpp"
#include <thread>
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
		xassert(per.parse(buf, strlen(buf)));
		xassert(per.get_method() == xhttper::parser::method::e_get);
		xassert(per.get_path() == "http://eclick.baidu.com/a.js?tu=u2310667&jk=3f4ff730444a9cb7&word=http%3A%2F%2Fe.firefoxchina.cn%2F%3Fcachebust%3D20160321&if=3&aw=250&ah=108&pt=96500&it=96500&vt=96500&csp=1920,1040&bcl=250,120&pof=250,120&top=0&left=0&total=1&rdm=1479249557254");
		xassert(per.get_version() == xhttper::parser::version::e_1_1);

		xassert(per.get_header("Host") == "eclick.baidu.com");
		xassert(per.get_header("User-Agent") == "Mozilla/5.0 (Windows NT 10.0; WOW64; rv:50.0) Gecko/20100101 Firefox/50.0");
		xassert(per.get_header("Accept") == "*/*");
		xassert(per.get_header("Accept-Language") == "zh-CN,zh;q=0.8,en-US;q=0.5,en;q=0.3");
		xassert(per.get_header("Accept-Encoding") == "gzip, deflate");
		xassert(per.get_header("Referer") == "http://fragment.firefoxchina.cn/html/main_baidu_cloud_250x108.html");
		xassert(per.get_header("Cookie") == "BAIDUID=9DF780D9B96413B1421F2758E92D4DEB:FG=1");
		xassert(per.get_header("Connection") == "keep-alive");
	}
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
		do
		{
			xassert(per.parse(buf, strlen(buf)));
			per.reset_status();
			count++;
		} while (1);
	}
}
