#pragma once
#include <string>
#include <map>
#include <list>
#include <sstream>
namespace xhttper
{
	class http_builder
	{
	public:
		http_builder()
		{

		}
		http_builder &set_version(const std::string & ver)
		{
			version_ = ver;
			return *this;
		}
		http_builder &set_status(int status)
		{
			status_ = status;
			return *this;
		}
		template<typename T>
		http_builder &append_entry(T &&key, T &&value)
		{
			headers_.emplace_back(std::forward<T>(key), std::forward<T>(value));
			return *this;
		}
		http_builder &append_entry(const char *key, const char *value)
		{
			headers_.emplace_back(key, value);
			return *this;
		}
		http_builder & set_url(const std::string &url)
		{
			url_ = url;
			return *this;
		}
		http_builder & set_method(const std::string &method)
		{
			method_ = method;
			return *this;
		}
		std::string build_resp()
		{
			std::string buffer;
			buffer.reserve(1024);
			buffer.append(version_);
			buffer.push_back(' ');
			buffer.append(std::to_string(status_));
			buffer.push_back(' ');
			buffer.append(get_status_str(status_));
			buffer.append("\r\n");
			for (auto &itr : headers_)
			{
				buffer.append(itr.first);
				buffer.append(": ");
				buffer.append(itr.second);
				buffer.append("\r\n");
			}
			buffer.append("\r\n");
			return std::move(buffer);
		}
		std::string build_req()
		{
			std::string buffer;
			buffer.reserve(1024);
			buffer.append(method_);
			buffer.push_back(' ');
			buffer.append(url_);
			buffer.append(version_);
			buffer.append("\r\n");
			for (auto &itr : headers_)
			{
				buffer.append(itr.first);
				buffer.append(": ");
				buffer.append(itr.second);
				buffer.append("\r\n");
			}
			buffer.append("\r\n");
			return std::move(buffer);
		}
		std::string encode_chunked(std::string && data)
		{
			std::ostringstream oss;
			oss << std::hex << data.size();
			oss << "\r\n" << data << "\r\n";
			return std::move(oss.str());
		}
		void reset()
		{
			headers_.clear();
			url_ = "/";
			method_ = "GET";
		}
	private:
		std::string get_status_str(int num)
		{
			static const std::map<int, std::string> status_map = {
				{100, "Continue"},
				{101, "Switching Protocols"},
				{102, "Processing"},

				{200, "OK"},
				{201, "Created"},
				{202, "Accepted"},
				{203, "Non-Authoritative Information"},
				{204, "No Content"},
				{205, "Reset Content"},
				{206, "Partial Content"},
				{300, "Multiple Choices"},
				{301, "Moved Permanently"},
				{302, "Move temporarily"},

				{303, "See Other"},
				{304, "Not Modified"},
				{305, "Use Proxy"},
				{306, "Switch Proxy"},
				{307, "Temporary Redirect"},
				{400, "Bad Request"},
				{401, "Unauthorized"},
				{403, "Forbidden"},
				{404, "Not Found"},
				{405, "Method Not Allowed"},
				{406, "Not Acceptable"},
				{407, "Proxy Authentication Required"},
				{408, "Request Timeout"},
				{409, "Conflict"},
				{410, "Gone"},
				{411, "Length Required"},

				{412, "Precondition Failed"},
				{413, "Request Entity Too Large"},
				{414, "Request-URI Too Long"},
				{415, "Unsupported Media Type"},
				{416, "Requested Range Not Satisfiable"},
				{417, "Expectation Failed"},
				{422, "Unprocessable Entity"},
				{423, "Locked"},
				{424, "Failed Dependency"},
				{425, "Unordered Collection"},
				{426, "Upgrade Required"},
				{449, "Retry With"},

				{500, "Internal Server Error"},
				{501, "Not Implemented"},
				{502, "Bad Gateway"},
				{503, "Service Unavailable"},
				{504, "Gateway Timeout"},
				{505, "HTTP Version Not Supported"},
				{506, "Variant Also Negotiates"},
				{507, "Insufficient Storage"},
				{509, "Bandwidth Limit Exceeded"},
				{510, "Not Extended"},
				{600, "Unparseable Response Headers"}};

			auto itr = status_map.find(num);
			if (itr == status_map.end())
				return{};
			return itr->second;
		}
		std::string url_ = "/";
		std::string method_="GET";
		std::list<std::pair<std::string, std::string>> headers_;
		std::string version_{ "HTTP/1.1" };
		int status_ = 200;
	};
}