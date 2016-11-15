#pragma once
#include <map>
namespace xhttper
{
	
	struct data_error : std::exception {};

	class xhttper
	{
	public:
		
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
		enum class version
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
		std::string get_header(const char *header_name)
		{
			std::size_t len = strlen(header_name);
			str_ref str;
			str.len_ = len;
			auto range = headers_.equal_range(str);
			for (auto itr = range.first; itr != range.second; ++itr)
			{
				if (strncmp(
					header_name, 
					buf_.data() + itr->first.pos_, 
					len) == 0)
				{
					return std::string(
						buf_.data() + itr->second.pos_, 
						itr->second.len_);
				}
			}
			return {};
		}
		method get_method()
		{
			return method_;
		}
		version get_version()
		{
			return version_;
		}
	private:
		struct str_ref
		{
			str_ref() {}
			str_ref(const str_ref&self)
			{
				pos_ = self.pos_;
				len_ = self.len_;
			}
			void operator= (const str_ref&self)
			{
				pos_ = self.pos_;
				len_ = self.len_;
			}
			str_ref(str_ref &&self)
			{
				pos_ = self.pos_;
				len_ = self.len_;
				self.reset();
			}
			void reset()
			{
				len_ = 0;
				pos_ = -1;
			}
			int pos_ = -1;
			int len_ = 0;
		};
		
		bool do_parser()
		{
			if (!parse_method())
				return false;
			if (!parse_path())
				return false;
			if (!parse_version())
				return false;
			if (!parse_headers())
				return false;
			return true;
		}
		bool parse_headers()
		{
			while (true)
			{
				if (!first_.len_)
				{
					auto ref = get_str_ref(':');
					if (!ref.len_)
						return false;
					first_ = ref;
					next();
					continue;
				}
				auto ref = get_str_ref('\r');
				if (!ref.len_)
					return false;
				if (next() != '\n')
					throw data_error();

				headers_.emplace(first_, ref);
				first_.reset();
				if (look_ahead(1) == '\r' && look_ahead(2) == '\n')
				{
					next();
					next();
					return true;
				}
			}
		}
		bool parse_version()
		{
			if (version_ != version::e_null)
				return true;
			if (!skip_space())
				return false;
			static constexpr int verlen = sizeof("HTTP/1.x") - 1;
			if (pos_ + verlen > buf_.size())
				return;
			auto ref = get_str_ref('.');
			auto text = std::string(buf_.data() + ref.pos_, ref.pos_);
			if (text != "HTTP/1")
				throw data_error();
			next();
			char v = next();
			if (v == 1)
				version_ = version::e_1_1;
			else if(v == 0)
				version_ = version::e_1_0;
			else 
				throw data_error();
		}
		bool parse_path()
		{
			if (path_.len_)
				return true;
			if (!skip_space())
				return false;
			auto ref = get_str_ref(' ');
			if (!ref.len_)
				return false;
			path_ = ref;
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
		bool parse_method()
		{
			if (method_ != method::e_null)
				return true;

			auto str = get_str_ref(' ');
			if (str.len_ == 0)
				return false;

			auto text = std::string(buf_.data() + str.pos_, str.len_);
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
				throw data_error();
			return true;
		}
		str_ref get_str_ref(char end)
		{
			if (str_ref_.pos_ == -1)
				str_ref_.pos_ = pos_;

			while (has_next())
			{
				if (curr() != end)
					next();
				else
					break;
			}
			if (!has_next())
				return str_ref_;

			str_ref_.len_ = pos_ - str_ref_.pos_;
			return std::move(str_ref_);
		}
		bool has_next()
		{
			return pos_ < buf_.size();
		}
		const char curr()
		{
			return buf_[pos_];
		}
		char look_ahead(int index)
		{
			if (pos_ + index >= buf_.size())
				return '\0';
			return buf_[pos_+index];
		}
		const char next()
		{
			return buf_[++pos_];
		}
		
		str_ref str_ref_;
		str_ref first_;

		method method_ = method::e_null;
		str_ref path_;
		version version_ = e_null;

		std::size_t last_pos_ = 0;
		std::size_t pos_= 0;
		std::string buf_;
		std::multimap<str_ref, str_ref> headers_;
	};
}
