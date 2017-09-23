#include "stdafx.h"

std::wstring ToUTF8(const std::string & str)
{
	try
	{
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		std::wstringstream strm(converter.from_bytes(str));
		return strm.str();
	}
	catch (std::exception & ex)
	{
		std::cout << ex.what() << "\n";
		return std::wstring();
	}
}

void ToUTF8(std::wofstream & stream)
{
	stream.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t, 0x10ffff, std::generate_header>));
}
