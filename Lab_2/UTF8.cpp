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
