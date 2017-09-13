#include "stdafx.h"
#include "UrlValidation.h"
using namespace std;

const int ARG_COUNT = 2;

void UseUTF16()
{
	_setmode(_fileno(stdout), _O_U8TEXT);
	_setmode(_fileno(stdin), _O_U8TEXT);
}

void PrintError(const wstring msg)
{
	wcout << L"Ошибка: " << msg << L"\n";
}

int wmain(int argc, wchar_t *argv[])
{
	UseUTF16();
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
