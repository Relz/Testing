#include "stdafx.h"
#include "Print.h"
using namespace std;

bool DoesUrlContainsColonAndSlashesOnce(const string & url)
{
	smatch match;
	regex_search(url, match, regex("://"));
	return match.size() == 1;
}

bool ValidProtocol(const string & url)
{
	return regex_search(url, regex("^https?:"));
}

bool DoesUrlContainsDomainZone(const string & url)
{
	return regex_search(url, regex("[.].+$"));
}

bool DoesUrlContainsDomainName(const string & url)
{
	return regex_search(url, regex("://.+[.]"));
}

bool DoesUrlContainsDoubleDot(const string & url)
{
	return regex_search(url, regex("[.][.]"));
}

bool DoesUrlContainsDotInEnd(const string & url)
{
	return regex_search(url, regex("[.]$"));
}

bool DoesUrlContainsEmptySubdomain(const string & url)
{
	return regex_search(url, regex("://[.]"));
}

bool ValidateURL(const string & url)
{
	if (!DoesUrlContainsColonAndSlashesOnce(url))
	{
		PrintlnError(u8"Неверный URL. После протокола должно следовать двоеточие с двумя косыми чертами");
		return false;
	}
	if (!ValidProtocol(url))
	{
		PrintlnError(u8"Неверный протокол. Доступны два протокола: http и https");
		return false;
	}
	if (!DoesUrlContainsDomainZone(url))
	{
		PrintlnError(u8"Неверный URL. Не указана доменная зона");
		return false;
	}
	if (!DoesUrlContainsDomainName(url))
	{
		PrintlnError(u8"Неверный URL. Не указано доменное имя");
		return false;
	}
	if (DoesUrlContainsDoubleDot(url))
	{
		PrintlnError(u8"Неверный URL. Две точки не могут следовать друг за другом");
		return false;
	}
	if (DoesUrlContainsDotInEnd(url))
	{
		PrintlnError(u8"Неверный URL. Точка не может заканчивать ссылку веб-страницы");
		return false;
	}
	if (DoesUrlContainsEmptySubdomain(url))
	{
		PrintlnError(u8"Неверный URL. Домен третьего уровня не может являться пустой строкой");
		return false;
	}
	return true;
}