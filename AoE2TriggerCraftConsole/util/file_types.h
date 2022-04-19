#pragma once
#include <string>
#include <vector>

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

	template <size_t n>
	class byte
	{
	public:
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
		T length;
		string s;
		void read(const char*& p_bin)
		{
			memcpy(&length, p_bin, sizeof(T));
			p_bin += sizeof(T);
			s = string(p_bin, length);
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

	template <typename T> void vector_cst(vector<T>& dst, const char*& src, size_t size)
	{
		if (size)
		{
			dst.resize(size);
			for (size_t i = 0; i < size; ++i)
			{
				dst[i].read(src);
			}
		}
	}
	template <size_t n> void vector_cst(vector<byte<n>>& dst, const char*& src, size_t size)
	{
		if (size)
		{
			dst.resize(size);
			data_read(dst[0], src, size * sizeof(byte<n>));
		}
	}
	void vector_cst(vector<uint8_t>& dst, const char*& src, size_t size);
	void vector_cst(vector<uint32_t>& dst, const char*& src, size_t size);
	void vector_cst(vector<int32_t>& dst, const char*& src, size_t size);
}
