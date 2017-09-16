#include "stdafx.h"
#include "UTF8.h"
#include "UrlValidation.h"
#include "Print.h"
using namespace std;

const int ARG_COUNT = 2;

int main(int argc, char *argv[])
{
	setlocale(LC_ALL, "Russian");
	if (argc != ARG_COUNT)
	{
		PrintlnError(u8"Укажите ссылку в качестве параметра. Формат ввода: check_links.exe <URL>");
		return 1;
	}
	string url = argv[1];
	if (!ValidateURL(url))
	{
		return 1;
	}

	return 0;
}
