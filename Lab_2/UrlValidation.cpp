#include "stdafx.h"
#include "UrlValidation.h"
using namespace std;

void PrintError(const wstring msg)
{
	wcout << L"Ошибка: " << msg << L"\n";
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

bool ValidateURL(const wstring & url)
{
	if (!DoesUrlContainsColonAndSlashesOnce(url))
	{
		PrintError(L"Неверный URL. После протокола должно следовать двоеточие с двумя косыми чертами");
		return false;
	}
	if (!ValidProtocol(url))
	{
		PrintError(L"Неверный протокол. Доступны два протокола: http и https");
		return false;
	}
	if (!DoesUrlContainsDomainZone(url))
	{
		PrintError(L"Неверный URL. Не указана доменная зона");
		return false;
	}
	if (!DoesUrlContainsDomainName(url))
	{
		PrintError(L"Неверный URL. Не указано доменное имя");
		return false;
	}
	if (DoesUrlContainsDoubleDot(url))
	{
		PrintError(L"Неверный URL. Две точки не могут следовать друг за другом");
		return false;
	}
	if (DoesUrlContainsDotInEnd(url))
	{
		PrintError(L"Неверный URL. Точка не может заканчивать ссылку веб-страницы");
		return false;
	}
	if (DoesUrlContainsEmptySubdomain(url))
	{
		PrintError(L"Неверный URL. Домен третьего уровня не может являться пустой строкой");
		return false;
	}
	if (!DoesDomainContainsOnlyAllowedCharacters(url))
	{
		PrintError(L"Неверный URL. Разрешённые символы домена: A-Z, a-z, 0-9, -, _, ., ~");
		return false;
	}
	return true;
}