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

void LoadArgumentsToStringStream(wstringstream & stringStream, wchar_t *argv[], int argCount)
{
	for (int i = 1; i < argCount; ++i)
	{
		stringStream << wstring(argv[i]) << L" ";
	}
}

bool GetTriangleLengths(wstringstream & stringStream, double &a, double &b, double &c)
{
	if (stringStream >> a >> b >> c)
	{
		return true;
	}
	return false;
}

int wmain(int argc, wchar_t *argv[])
{
	UseUTF16();
	if (argc != ARG_COUNT)
	{
		PrintError(L"Укажите длины сторон в качестве параметров. Формат ввода: triangle.exe a b c");
		return 1;
	}
	wstringstream stringStream;
	LoadArgumentsToStringStream(stringStream, argv, ARG_COUNT);
	double a = 0;
	double b = 0;
	double c = 0;
	if (!GetTriangleLengths(stringStream, a, b, c))
	{
		PrintError(L"Длины треугольника должны быть числами");
		return 1;
	}
	return 0;
}
