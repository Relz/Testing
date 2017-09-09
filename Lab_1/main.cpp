#include "stdafx.h"
using namespace std;

const int ARG_COUNT = 4;

void PrintError(wstring msg)
{
	wcout << L"Ошибка: " << msg << L"\n";
}

void UseUTF16()
{
	_setmode(_fileno(stdout), _O_U16TEXT);
	_setmode(_fileno(stdin), _O_U16TEXT);
	_setmode(_fileno(stderr), _O_U16TEXT);
}

int wmain(int argc, wchar_t *argv[])
{
	UseUTF16();
	if (argc != ARG_COUNT)
	{
		PrintError(L"Укажите длины сторон в качестве параметров. Формат ввода: triangle.exe a b c");
		return 1;
	}
	return 0;
}