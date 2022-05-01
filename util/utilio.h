#pragma once
#include <stdexcept>
#include <fstream>
#include "libdeflate/libdeflate.h"

template<class... T>
std::string make_fmt_string(const char* fmt, const T... args)
{
	char buf[256];
	sprintf(buf, fmt, args...);
	return std::string(buf);
}

template<class... T>
std::wstring make_fmt_string(const wchar_t* fmt, const T... args)
{
	wchar_t buf[256];
	wsprintf(buf, fmt, args...);
	return std::wstring(buf);
}

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
	AutoFile(const wchar_t* filename, std::ios_base::openmode mode);
	~AutoFile();

	std::fstream& operator*();
	std::fstream* operator->();

private:
	AutoFile(const AutoFile&); // no copy constructor!

	std::fstream _file;
};