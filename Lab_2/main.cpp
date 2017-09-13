#include "stdafx.h"
#include "UTF8.h"
#include "UrlValidation.h"
using namespace std;

const int ARG_COUNT = 2;

void PrintError(const wstring msg)
{
	wcout << L"Ошибка: " << msg << L"\n";
}

int wmain(int argc, wchar_t *argv[])
{
	UseUTF8();
	if (argc != ARG_COUNT)
	{
		PrintError(L"Укажите ссылку в качестве параметра. Формат ввода: check_links.exe <URL>");
		return 1;
	}
	wstring url = argv[1];
	if (ValidateURL(url))
	{
		return 1;
	}

	return 0;
}
