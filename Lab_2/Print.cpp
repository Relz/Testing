#include "stdafx.h"
#include "UTF8.h"

void Print(const std::string & msg)
{
	std::wcout << ToUTF8(msg);
}

void Println(const std::string & msg)
{
	Print(msg);
	Print(u8"\n");
}

void PrintlnError(const std::string & msg)
{
	Print(u8"Ошибка: ");
	Print(msg);
	Print(u8"\n");
}
