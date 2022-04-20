#include "utilio.h"

DeflateClass::DeflateClass() :
	compressor(libdeflate_alloc_compressor(6)),
	decompressor(libdeflate_alloc_decompressor())
{}
DeflateClass::~DeflateClass()
{
	libdeflate_free_compressor(compressor);
	libdeflate_free_decompressor(decompressor);
}
std::size_t DeflateClass::decompress(const void* in, std::size_t in_nbytes, void* out, std::size_t out_nbytes_avail)
{
	std::size_t actual_out_nbytes_ret = 0;
	auto ret = libdeflate_deflate_decompress(decompressor, in, in_nbytes, out, out_nbytes_avail, &actual_out_nbytes_ret);
	switch (ret)
	{
	case LIBDEFLATE_SUCCESS:
		break;
	case LIBDEFLATE_BAD_DATA:
		throw std::runtime_error("Deflate bad data.");
		break;
	case LIBDEFLATE_SHORT_OUTPUT:
		throw std::runtime_error("Deflate short output.");
		break;
	case LIBDEFLATE_INSUFFICIENT_SPACE:
		throw std::runtime_error("Deflate no space.");
		break;
	default:
		break;
	}
	return actual_out_nbytes_ret;
}
std::size_t DeflateClass::compress(const void* in, std::size_t in_nbytes, void* out, std::size_t out_nbytes_avail)
{
	auto ret = libdeflate_deflate_compress(compressor, in, in_nbytes, out, out_nbytes_avail);
	if (ret == 0)
	{
		throw std::runtime_error("Deflate compress fail.");
	}
	return ret;
}

AutoFile::AutoFile(const char* filename, std::ios_base::openmode mode)
	:_file(filename, mode)
{
	if (_file.fail())
	{
		throw std::runtime_error("Can not open file.");
	}
}
AutoFile::~AutoFile()
{
	if (_file.is_open())
	{
		_file.close();
	}
}

std::fstream* AutoFile::operator->()
{
	return &_file;
}