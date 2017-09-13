#include "stdafx.h"
using namespace std;

const int ARG_COUNT = 2;

void PrintError(const wstring msg)
{
	wcout << L"Ошибка: " << msg << L"\n";
}

void UseUTF16()
{
	_setmode(_fileno(stdout), _O_U8TEXT);
	_setmode(_fileno(stdin), _O_U8TEXT);
}

bool DoesUrlContainsColonAndSlashesOnce(const wstring & url)
{
	wsmatch match;
	regex_search(url, match, wregex(L"://"));
	return match.size() == 1;
}

bool ValidProtocol(const wstring & url)
{
	return regex_search(url, wregex(L"^https?:"));
}

bool DoesUrlContainsDomainZone(const wstring & url)
{
	return regex_search(url, wregex(L"[.].+$"));
}

bool DoesUrlContainsDomainName(const wstring & url)
{
	return regex_search(url, wregex(L"://.+[.]"));
}

bool DoesUrlContainsDoubleDot(const wstring & url)
{
	return regex_search(url, wregex(L"[.][.]"));
}

bool DoesUrlContainsDotInEnd(const wstring & url)
{
	return regex_search(url, wregex(L"[.]$"));
}

bool DoesUrlContainsEmptySubdomain(const wstring & url)
{
	return regex_search(url, wregex(L"://[.]"));
}

bool DoesDomainContainsOnlyAllowedCharacters(const wstring & url)
{
	return regex_search(url, wregex(L"://[A-Za-z0-9-_.~]+(\\?)|(/)"));
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
	if (!DoesUrlContainsColonAndSlashesOnce(url))
	{
		PrintError(L"Неверный URL. После протокола должно следовать двоеточие с двумя косыми чертами");
		return 1;
	}
	if (!ValidProtocol(url))
	{
		PrintError(L"Неверный протокол. Доступны два протокола: http и https");
		return 1;
	}
	if (!DoesUrlContainsDomainZone(url))
	{
		PrintError(L"Неверный URL. Не указана доменная зона");
		return 1;
	}
	if (!DoesUrlContainsDomainName(url))
	{
		PrintError(L"Неверный URL. Не указано доменное имя");
		return 1;
	}
	if (DoesUrlContainsDoubleDot(url))
	{
		PrintError(L"Неверный URL. Две точки не могут следовать друг за другом");
		return 1;
	}
	if (DoesUrlContainsDotInEnd(url))
	{
		PrintError(L"Неверный URL. Точка не может заканчивать ссылку веб-страницы");
		return 1;
	}
	if (DoesUrlContainsEmptySubdomain(url))
	{
		PrintError(L"Неверный URL. Домен третьего уровня не может являться пустой строкой");
		return 1;
	}
	if (!DoesDomainContainsOnlyAllowedCharacters(url))
	{
		PrintError(L"Неверный URL. Разрешённые символы домена: A-Z, a-z, 0-9, -, _, ., ~");
		return 1;
	}
	return 0;
}