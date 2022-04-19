
#include "file_types.h"

namespace AoE2ScenarioFileTypesNamespace
{
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
