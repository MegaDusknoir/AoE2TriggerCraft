#pragma once
#include <string>
#include <vector>
#include <deque>
#include <memory>

#define MAX_PLAYER_COUNT 16

namespace AoE2ScenarioFileTypesNamespace
{
	using std::string;
	using std::stringstream;
	using std::fstream;
	using std::ofstream;
	using std::ifstream;
	using std::ios;
	using std::size_t;
	using std::vector;
	using std::deque;
	using std::unique_ptr;

	template <size_t n>
	class bytes
	{
	public:
		bytes()
		{
			memset(value, 0, n * sizeof(uint8_t)); 
		}
		uint8_t value[n];
		uint8_t& operator[](size_t n)
		{
			return value[n];
		}
		uint8_t* operator&()
		{
			return value;
		}
	};

	template <typename T>
	class vStr
	{
	public:
		vStr() :length(1) { s += '\0'; }
		vStr(string&& _s) :length(_s.size()), s(_s) {}
		T length;
		string s;
		operator string()
		{
			string ret = s;
			if (!ret.empty() && ret.back() == '\0')
			{
				ret.pop_back();
			}
			return ret;
		}
		vStr& operator=(string&& rhs)
		{
			bool trail_zero = (!s.empty() && s.back() == '\0');
			s = std::move(rhs);
			if (trail_zero)
			{
				s.push_back('\0');
			}
			length = s.size();
			return *this;
		}
		vStr& operator=(const string& rhs)
		{
			bool trail_zero = (!s.empty() && s.back() == '\0');
			s = rhs;
			if (trail_zero)
			{
				s.push_back('\0');
			}
			length = s.size();
			return *this;
		}
		vStr& operator+=(const string& rhs)
		{
			bool trail_zero = (!s.empty() && s.back() == '\0');
			if (trail_zero)
			{
				s.pop_back();
			}
			s += rhs;
			if (trail_zero)
			{
				s.push_back('\0');
			}
			length += rhs.size();
			return *this;
		}
		vStr operator+(const string& rhs)
		{
			vStr temp(s);
			return temp += rhs;
		}
		string cr_to_crlf() const
		{
			string s_cr;
			size_t pos = 0;
			while (1)
			{
				size_t nxt = s.find('\r', pos);
				if (nxt != s.npos)
				{
					s_cr.append(s.begin() + pos, s.begin() + nxt);
					s_cr.append("\r\n");
					pos = nxt + 1;
				}
				else
				{
					s_cr.append(s.begin() + pos, s.end());
					break;
				}
			}
			return s_cr;
		}
		void convert_from_crlf(const string src)
		{
			string s_cr;
			size_t pos = 0;
			while (1)
			{
				size_t nxt = src.find('\n', pos);
				if (nxt != src.npos)
				{
					s_cr.append(src.begin() + pos, src.begin() + nxt);
					pos = nxt + 1;
				}
				else
				{
					s_cr.append(src.begin() + pos, src.end());
					break;
				}
			}
			*this = std::move(s_cr);
		}
		void read(const char*& p_bin)
		{
			memcpy(&length, p_bin, sizeof(T));
			p_bin += sizeof(T);
			s = string(p_bin, length);
			p_bin += length;
		}
		void write(char*& p_bin) const
		{
			memcpy(p_bin, &length, sizeof(T));
			p_bin += sizeof(T);
			memcpy(p_bin, s.data(), length);
			p_bin += length;
		}
	};
	using str16 = vStr<uint16_t>;
	using str32 = vStr<uint32_t>;

	template <typename T> void data_read(T& dst, const char*& src, size_t size = sizeof(T))
	{
		memcpy(&dst, src, size);
		src += size;
	}
	template <typename T> void data_read(vStr<T>& dst, const char*& src)
	{
		dst.read(src);
	}

	template <typename T> void data_write(const T& src, char*& dst, size_t size = sizeof(T))
	{
		memcpy(dst, &src, size);
		dst += size;
	}
	template <typename T> void data_write(const vStr<T>& src, char*& dst)
	{
		src.write(dst);
	}
	template <typename T> void data_write(const vector<T>& src, char*& dst)
	{
		if (src.size())
		{
			for (size_t i = 0; i < src.size(); ++i)
			{
				src[i].write(dst);
			}
		}
	}
	template <size_t n> void data_write(const vector<bytes<n>>& src, char*& dst)
	{
		if (src.size())
		{
			size_t size = src.size() * sizeof(bytes<n>);
			memcpy(dst, &src[0], size);
			dst += size;
		}
	}
	void data_write(const vector<uint8_t>& src, char*& dst);
	void data_write(const vector<uint32_t>& src, char*& dst);
	void data_write(const vector<int32_t>& src, char*& dst);

	template <typename T> void vector_cst(vector<T>& dst, const char*& src, size_t size)
	{
		dst.resize(size);
		if (size)
		{
			for (size_t i = 0; i < size; ++i)
			{
				dst[i].read(src);
			}
		}
	}
	template <size_t n> void vector_cst(vector<bytes<n>>& dst, const char*& src, size_t size)
	{
		dst.resize(size);
		if (size)
		{
			data_read(dst[0], src, size * sizeof(bytes<n>));
		}
	}
	void vector_cst(vector<uint8_t>& dst, const char*& src, size_t size);
	void vector_cst(vector<uint32_t>& dst, const char*& src, size_t size);
	void vector_cst(vector<int32_t>& dst, const char*& src, size_t size);
}
