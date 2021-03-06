#include "stdafx.h"
using namespace std;

const int ARG_COUNT = 4;

void PrintError(const wstring & msg)
{
	wcout << L"Ошибка: " << msg << L"\n";
}

void UseUTF8()
{
	_setmode(_fileno(stdout), _O_U8TEXT);
	_setmode(_fileno(stdin), _O_U8TEXT);
}

void LoadArgumentsToStringStream(wstringstream & wstringStream, wchar_t *argv[], int argCount)
{
	for (int i = 1; i < argCount; ++i)
	{
		wstringStream << wstring(argv[i]) << L" ";
	}
}

bool IsValidArgument(const wstring & numberStr)
{
	bool dotProcessed = false;
	if (numberStr.length() == 0)
	{
		return false;
	}
	for (wchar_t wch : numberStr)
	{
		if (wch < L'0' || wch > L'9')
		{
			if (wch == L'.' && !dotProcessed)
			{
				dotProcessed = true;
				continue;
			}
			return false;
		}
	}
	return true;
}

bool AreValidArguments(wchar_t *argv[], int argCount)
{
	for (int i = 1; i < argCount; ++i)
	{
		if (!IsValidArgument(argv[i]))
		{
			return false;
		}
	}
	return true;
}

int wmain(int argc, wchar_t *argv[])
{
	UseUTF8();
	if (argc != ARG_COUNT)
	{
		PrintError(L"Укажите длины сторон в качестве параметров. Формат ввода: triangle.exe a b c");
		return 1;
	}
	if (!AreValidArguments(argv, ARG_COUNT))
	{
		PrintError(L"Длины треугольника должны быть числами. Для разделения целой части числа от дробной используйте точку");
		return 1;
	}
	wstringstream wstringStream;
	LoadArgumentsToStringStream(wstringStream, argv, ARG_COUNT);
	double a = 0;
	double b = 0;
	double c = 0;
	if (!(wstringStream >> a >> b >> c))
	{
		PrintError(L"Тип данных для хранения длины треугольника не может превышать " + to_wstring(numeric_limits<double>::max()));
		return 1;
	}
	if (a < 0 || b < 0 || c < 0)
	{
		PrintError(L"Длины треугольника должны быть положительными числами");
		return 1;
	}
	if (a == 0 || b == 0 || c == 0 || (a + b) <= c || (a + c) <= b || (b + c) <= a)
	{
		wcout << L"Не треугольник";
		return 0;
	}
	if (a != b && b != c && a != c)
	{
		wcout << L"Обычный";
		return 0;
	}
	if (a == b && b == c && a == c)
	{
		wcout << L"Равносторонний";
		return 0;
	}
	if (a == b || b == c || a == c)
	{
		wcout << L"Равнобедренный";
		return 0;
	}
	return 0;
}
