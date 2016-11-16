#pragma once
#include <string>
#include <map>
#include <list>
#include <sstream>
namespace xhttper
{
	class builder
	{
	public:
		builder()
		{

		}
		builder &set_version(const std::string & ver)
		{
			version_ = ver;
			return *this;
		}
		builder &set_status(const std::string & status)
		{
			status_ = status;
			return *this;
		}
		template<typename T>
		builder &append_header(T &&key, T &&value)
		{
			headers_.emplace_back(std::forward<T>(key), std::forward<T>(value));
			return *this;
		}
		builder &append_header(const char *key, const char *value)
		{
			headers_.emplace_back(key, value);
			return *this;
		}
		std::string build()
		{
			std::string buffer_;
			buffer_.reserve(1024);
			buffer_.append(version_);
			buffer_.push_back(' ');
			buffer_.append(status_);
			buffer_.push_back(' ');
			buffer_.append(get_status_str(status_));
			buffer_.append("\r\n");
			for (auto &itr: headers_)
			{
				buffer_.append(itr.first);
				buffer_.append(": ");
				buffer_.append(itr.second);
				buffer_.append("\r\n");
			}
			buffer_.append("\r\n");
			return std::move(buffer_);
		}
		std::string encode_chunked(std::string && data)
		{
			std::ostringstream oss;
			oss << std::hex << data.size();
			oss << "\r\n" << data << "\r\n";
			return std::move(oss.str());
		}
	private:
		std::string get_status_str(const std::string &status)
		{
			static const std::map<std::string, std::string> status_map = { 
				{"200", "OK"},
				{"201", "Created"},
				{"202", "Accepted"},
				{"204", "No Content"},
				{"300", "Multiple Choices"},
				{"301", "Moved Permanently"},
				{"302", "Moved Temporarily"},
				{"304", "Not Modified"},
				{"400", "Bad Request"},
				{"401", "Unauthorized"},
				{"403", "Forbidden"},
				{"404", "Not Found"}, 
				{"500", "Internal Server Error"},
				{"501", "Not Implemented"},
				{"502", "Bad Gateway"}, 
				{"503", "Service Unavailable"}};
			auto itr = status_map.find(status);
			if (itr == status_map.end())
				return {};
			return itr->second;
		}
		std::list<std::pair<std::string, std::string>> headers_;
		std::string version_{ "HTTP/1.1" };
		std::string status_{ "200" };
		
	};
}