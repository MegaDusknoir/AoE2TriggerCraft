#pragma once
#include <stdexcept>
#include <fstream>
#include "libdeflate/libdeflate.h"

class DeflateClass
{
public:
	DeflateClass();
	~DeflateClass();
public:
	std::size_t decompress(const void* in, std::size_t in_nbytes, void* out, std::size_t out_nbytes_avail);
	std::size_t compress(const void* in, std::size_t in_nbytes, void* out, std::size_t out_nbytes_avail);
private:
	libdeflate_compressor *compressor;
	libdeflate_decompressor *decompressor;
};

class AutoFile
{
public:
	AutoFile(const char* filename, std::ios_base::openmode mode);
	~AutoFile();

	std::fstream* operator->();

private:
	AutoFile(const AutoFile&); // no copy constructor!

	std::fstream _file;
};