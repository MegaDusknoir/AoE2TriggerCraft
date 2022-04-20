
#include "file_types.h"

namespace AoE2ScenarioFileTypesNamespace
{
	void data_write(const vector<uint8_t>& src, char*& dst)
	{
		if (src.size())
		{
			size_t size = src.size() * sizeof(uint8_t);
			memcpy(dst, &src[0], size);
			dst += size;
		}
	}
	void data_write(const vector<uint32_t>& src, char*& dst)
	{
		if (src.size())
		{
			size_t size = src.size() * sizeof(uint32_t);
			memcpy(dst, &src[0], size);
			dst += size;
		}
	}
	void data_write(const vector<int32_t>& src, char*& dst)
	{
		if (src.size())
		{
			size_t size = src.size() * sizeof(int32_t);
			memcpy(dst, &src[0], size);
			dst += size;
		}
	}
	void vector_cst(vector<uint8_t>& dst, const char*& src, size_t size)
	{
		if (size)
		{
			dst.resize(size);
			data_read(dst[0], src, size * sizeof(uint8_t));
		}
	}
	void vector_cst(vector<uint32_t>& dst, const char*& src, size_t size)
	{
		if (size)
		{
			dst.resize(size);
			data_read(dst[0], src, size * sizeof(uint32_t));
		}
	}
	void vector_cst(vector<int32_t>& dst, const char*& src, size_t size)
	{
		if (size)
		{
			dst.resize(size);
			data_read(dst[0], src, size * sizeof(int32_t));
		}
	}
}
