#pragma once
#include <map>
namespace xhttper
{
	
	struct parse_error : std::exception {};

	class xhttper
	{
	public:
		struct str_ref
		{
			std::size_t pos_;
			std::size_t len_;
		};
		enum class method
		{
			e_null,
			e_get,
			e_post,
			e_put,
			e_head,
			e_delete,
			e_connect,
			e_trace,
			e_options,
		};
		enum version
		{
			e_null,
			e_1_0,
			e_1_1
		};
		xhttper()
		{
			buf_.reserve(1024);
		}
		bool parse(const char *dat, std::size_t len)
		{
			buf_.append(dat, len);
			return do_parser();
		}
	private:
		/*
		GET /download.google.com/somedata.exe  HTTP/1.1
		Host: download.google.com
		Accept:
		Pragma : no - cache
		Cache-Control : no - cache
		Referer : http://download.google.com/ 
		User-Agent : Mozilla / 4.04[en](Win95; I; Nav)
		Range : bytes = 554554 -
		*/
		bool do_parser()
		{
			if (!get_method())
				return false;
			if (!get_path())
				return false;
		}
		bool get_path()
		{
			if (!skip_space())
				return false;

		}
		bool skip_space()
		{
			while (pos_ < buf_.size() )
			{
				char ch = buf_[pos_];
				if (ch == ' ' ||
					ch == '\t' ||
					ch == '\r' ||
					ch == '\r')
					pos_++;
				else
					return true;
			}
			return false;
		}
		bool get_method()
		{
			if (method_ == method::e_null)
			{
				set_rollback();
				auto text = get_str(' ');
				if (text == "POST")
					method_ = method::e_post;
				else if (text == "GET")
					method_ = method::e_get;
				else if (text == "PUT")
					method_ = method::e_put;
				else if (text == "HEAD")
					method_ = method::e_head;
				else if (text == "CONNECT")
					method_ = method::e_connect;
				else if (text == "TRACE")
					method_ = method::e_trace;
				else if (text == "OPTIONS")
					method_ = method::e_options;

				if (method_ == method::e_null)
				{
					if (pos_ == buf_.size())
					{
						rollback(); 
						return false;
					}
					else
						throw parse_error();
				}
			}
			return true;
		}
		std::string get_str(char end)
		{
			std::string result;
			while (pos_ < buf_.size() && buf_[pos_] != end)
			{
				result.push_back(end);
			}
			return std::move(result);
		}
		void set_rollback()
		{
			last_pos_ = pos_;
		}
		void rollback()
		{
			pos_ = last_pos_;
		}
		

		method method_ = method::e_null;
		str_ref path_;
		version version_ = e_null;

		std::size_t last_pos_ = 0;
		std::size_t pos_= 0;
		std::string buf_;
		std::map<str_ref, str_ref> headers_;
	};
}
