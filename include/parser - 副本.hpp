#pragma once
#include <map>
#include <string>
#include <vector>
namespace xhttper
{
	
	struct parse_error : std::exception {};

	class parser
	{
	public:
		parser()
		{
			buf_.reserve(1024);
		}
		void append(const char *dat, std::size_t len)
		{
			buf_.append(dat, len);
			size_ = buf_.size();
			ptr_ = (char*)buf_.data();
		}
		bool parse_req()
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
		
		bool parse_rsp()
		{
			if (!parse_version())
				return false;
			if (!parse_status())
				return false;
			if (!parse_status_str())
				return false;
			if (!parse_headers())
				return false;
			return true;
		}
		template<typename strncasecmper>
		std::string get_header(const char *header_name)
		{
			std::size_t len = strlen(header_name);
			for (auto &itr: headers_)
			{
				if (itr.first.len_ != len)
					continue;
				if (strncasecmper()(itr.first.to_string(buf_).data(), header_name, len))
					return itr.second.to_string(buf_);
			}
			return {};
		}
		std::string  get_method()
		{
			return method_.to_string(buf_);
		}
		std::string get_version()
		{
			return version_.to_string(buf_);
		}
		std::string get_status()
		{
			return status_.to_string(buf_);
		}
		std::string get_status_str()
		{
			return status_str_.to_string(buf_);
		}
		std::string get_path()
		{
			return std::string(buf_.data() + path_.pos_, path_.len_);
		}
		void reset()
		{
			if (pos_ == buf_.size())
				buf_.clear();
			else
				buf_ = std::move(buf_.substr(pos_, buf_.size() - pos_));
			pos_ = last_pos_ = 0;
			first_.reset();
			str_ref_.reset();
			headers_.clear();
			path_.reset();
			method_.reset();
			version_.reset();
			status_.reset();
			status_str_.reset();
		}
		std::string get_string()
		{
			return std::move(buf_);
		}
		std::string get_string(std::size_t len)
		{
			if (len > buf_.size())
				throw std::exception("string subscript out of range");
			std::string result(buf_.data(), len);
			buf_ = std::move(buf_.substr(len, buf_.size() - len));
			buf_.reserve(1024);
			return std::move(result);
		}
		std::size_t remain_len()
		{
			return buf_.size();
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
			std::string to_string(const std::string& buf)
			{
				return std::string(buf.data() + pos_, len_);
			}
			void reset()
			{
				len_ = 0;
				pos_ = -1;
			}
			int pos_ = -1;
			int len_ = 0;
		};
		struct str_ref_cmp
		{
			bool operator ()(const str_ref &left,const str_ref &right)const
			{
				return left.len_ < right.len_;
			}
		};
		bool parse_headers()
		{
			while (true)
			{
				if (!skip_space())
					return false;
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
					throw parse_error();

				headers_.emplace_back(first_, ref);
				first_.reset();
				if (look_ahead(1) == '\r' && look_ahead(2) == '\n')
				{
					next();
					next();
					next();
					return true;
				}
			}
		}
		bool parse_status()
		{
			if (status_.len_)
				return true;
			if (!skip_space())
				return false;
			auto ref = get_str_ref(' ');
			if (!ref.len_)
				return false;
			status_ = ref;
			return true;
		}
		bool parse_status_str()
		{
			if (status_str_.len_)
				return true;
			if (!skip_space())
				return false;
			auto ref = get_str_ref('\r');
			status_str_ = ref;
			return true;
		}
		bool parse_version()
		{
			if (version_.len_)
				return true;
			if (!skip_space())
				return false;
			static constexpr size_t len = sizeof("HTTP/1.1")-1;
			if (size_ - pos_ < len)
				return false;
			auto strref = get_str_ref('.');
			if (!strref.len_)
				return false;
			strref.len_ += 2;
			version_ = strref;
			next();
			next();
			return true;
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
			return true;
		}
		bool skip_space()
		{
			while (pos_ < size_ )
			{
				char ch = ptr_[pos_];
				if (ch == ' ' ||
					ch == '\t' ||
					ch == '\r' ||
					ch == '\n')
					pos_++;
				else
					return true;
			}
			return false;
		}
		bool parse_method()
		{
			if (method_.len_)
				return true;
			auto refstr = get_str_ref(' ');
			if (refstr.len_ == 0)
				return false;
			method_ = refstr;
			return true;
		}
		str_ref get_str_ref(char end)
		{
			if (str_ref_.pos_ == -1)
				str_ref_.pos_ = static_cast<int>(pos_);

			while (has_next())
			{
				if (curr() != end)
					next();
				else
					break;
			}
			if (!has_next())
				return str_ref_;

			str_ref_.len_ = static_cast<int>(pos_ - str_ref_.pos_);
			return std::move(str_ref_);
		}
		bool has_next()
		{
			return pos_ < size_;
		}
		const char curr()
		{
			return ptr_[pos_];
		}
		char look_ahead(int index)
		{
			if (pos_ + index >= size_)
				return '\0';
			return ptr_[pos_+index];
		}
		const char next()
		{
			return ptr_[++pos_];
		}

		str_ref  status_;
		str_ref status_str_;

		str_ref str_ref_;
		str_ref first_;

		str_ref method_ ;
		str_ref path_;
		str_ref version_ ;

		std::size_t last_pos_ = 0;
		std::size_t pos_= 0;
		std::size_t size_ = 0;
		std::string buf_;
		char *ptr_;
		std::vector<std::pair<str_ref, str_ref>> headers_;
	};
}
