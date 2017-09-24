#include "stdafx.h"
#include "UTF8.h"
#include "UrlValidation.h"
#include "Print.h"
#include "UrlParts.h"
using namespace std;

const int ARG_COUNT = 2;

string GetDMYDateTimeString(struct tm *now)
{
	return to_string(now->tm_mday) + "." + to_string(now->tm_mon + 1) + "." + to_string(now->tm_year + 1900) + "/"
		+ to_string(now->tm_hour) + ":" + to_string(now->tm_min) + ":" + to_string(now->tm_sec);
}

void PrintInfoToFile(size_t urlCount, struct tm *now, wofstream & outFile)
{
	Print(u8"Количество ссылок: ", outFile);
	Println(to_string(urlCount), outFile);
	Print(u8"Дата/время завершения проверки: ", outFile);
	Println(GetDMYDateTimeString(now), outFile);
}

static int Writer(char *data, size_t size, size_t nmemb, string *buffer) {
	int result = 0;
	if (buffer != NULL)
	{
		buffer->append(data, size * nmemb);
		result = size * nmemb;
	}

	return result;
}

CURLcode GetHtml(CURL **curl, const string & url, const string & html, char (&errorBuffer)[CURL_ERROR_SIZE])
{
	curl_easy_setopt(*curl, CURLOPT_ERRORBUFFER, errorBuffer);
	curl_easy_setopt(*curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(*curl, CURLOPT_FOLLOWLOCATION, true);
	curl_easy_setopt(*curl, CURLOPT_HEADER, true);
	curl_easy_setopt(*curl, CURLOPT_WRITEFUNCTION, Writer);
	curl_easy_setopt(*curl, CURLOPT_WRITEDATA, &html);
	return curl_easy_perform(*curl);
}

void GetHrefs(const string & html, const function<void(const sregex_iterator &)> & callback)
{
	regex re("href[ ]*=[ ]*[\"'].*?[\"']");
	for (auto it = sregex_iterator(html.begin(), html.end(), re); it != sregex_iterator(); ++it)
	{
		callback(it);
	}
}

bool GetUrl(const string & href, string & url)
{
	bool result = false;
	smatch resultWithQuotes;
	if (regex_search(href, resultWithQuotes, regex("[\"'].*?[\"']")))
	{
		result = true;
	}
	url = resultWithQuotes.str();
	url.erase(url.begin());
	url.erase(--url.end());
	return result;
}

long GetResponseCode(CURL **curl)
{
	long responseCode;
	curl_easy_getinfo(*curl, CURLINFO_RESPONSE_CODE, &responseCode);

	return responseCode;
}

bool UrlContainsMailtoOrTel(const string & url)
{
	return regex_search(url, smatch(), regex("^mailto:")) || regex_search(url, smatch(), regex("^tel:"));
}

bool ProcessUrl(
	CURL **curl,
	const CUrlParts & currentUrlParts,
	wofstream & urlsStatus,
	wofstream & badUrls,
	queue<CUrlParts*> & queue,
	unordered_set<string> & processedUrls,
	size_t & totalUrlCount,
	size_t & badUrlCount)
{
	string currentFullUrl;
	CUrlParts::CreateFullUrl(currentUrlParts, currentFullUrl);
	Println(u8"----------");
	Println(currentFullUrl);
	Println(u8"Получение HTML кода страницы");
	char errorBuffer[CURL_ERROR_SIZE];
	string html;
	if (GetHtml(curl, currentFullUrl, html, errorBuffer) != CURLE_OK)
	{
		PrintlnError(errorBuffer);
		return false;
	}
	long responseCode = GetResponseCode(curl);
	string responseCodeStr = to_string(responseCode);
	if (responseCode != 200)
	{
		++badUrlCount;
		Println(currentFullUrl + u8" : " + responseCodeStr, badUrls);
	}
	++totalUrlCount;
	Println(currentFullUrl + u8" : " + responseCodeStr, urlsStatus);
	Println(u8"Код ответа: " + responseCodeStr);

	Print(u8"Получение внутренних ссылок страницы: ");
	size_t urlCount = 0;
	GetHrefs(html, [&currentUrlParts, &processedUrls, &queue, &urlCount](const sregex_iterator & it) {
		string url;
		if (!GetUrl(it->str(), url))
		{
			return;
		}
		if (UrlContainsMailtoOrTel(url))
		{
			return;
		}
		CUrlParts *urlParts = new CUrlParts(url);
		if (urlParts->GetProtocol().empty())
		{
			urlParts->SetProtocol(currentUrlParts.GetProtocol());
		}
		if (urlParts->GetSubDomainName().empty())
		{
			urlParts->SetSubDomainName(currentUrlParts.GetSubDomainName());
		}
		if (urlParts->GetDomainName().empty())
		{
			urlParts->SetDomainName(currentUrlParts.GetDomainName());
		}
		if (urlParts->GetDomainName() != currentUrlParts.GetDomainName())
		{
			return;
		}
		string fullUrl;
		CUrlParts::CreateFullUrl(*urlParts, fullUrl);
		if (processedUrls.find(fullUrl) == processedUrls.end())
		{
			processedUrls.insert(fullUrl);
			queue.push(urlParts);
			++urlCount;
		}
	});
	Println(to_string(urlCount));
	Println(u8"----------");
	return true;
}

int main(int argc, char *argv[])
{
	setlocale(LC_ALL, "Russian");
	if (argc != ARG_COUNT)
	{
		PrintlnError(u8"Укажите ссылку в качестве параметра. Формат ввода: check_links.exe <URL>");
		return 1;
	}
	string sourceUrl = argv[1];
	if (!ValidateURL(sourceUrl))
	{
		return 1;
	}

	CURL *curl = curl_easy_init();
	if (curl == nullptr)
	{
		PrintlnError(u8"Не удалось инициализировать библиотеку для работы с сетью");
		return 1;
	}

	wofstream urlsStatus("urls_status.txt");
	wofstream badUrls("bad_urls.txt");
	ToUTF8(urlsStatus);
	ToUTF8(badUrls);
	
	CUrlParts *sourceUrlParts = new CUrlParts(sourceUrl);
	string fullUrl;
	CUrlParts::CreateFullUrl(*sourceUrlParts, fullUrl);

	unordered_set<string> processedUrls;
	processedUrls.insert(fullUrl);

	size_t totalUrlCount = 0;
	size_t badUrlCount = 0;
	queue<CUrlParts*> queue;
	queue.push(sourceUrlParts);
	while (!queue.empty())
	{
		Print(u8"Осталось обработать ссылок: ");
		Println(to_string(queue.size()));
		CUrlParts *currentUrlParts = queue.front();
		queue.pop();
		ProcessUrl(&curl, *currentUrlParts, urlsStatus, badUrls, queue, processedUrls, totalUrlCount, badUrlCount);
	}

	time_t currentTime = time(0);
	struct tm *now = localtime(&currentTime);
	PrintInfoToFile(totalUrlCount, now, urlsStatus);
	PrintInfoToFile(badUrlCount, now, badUrls);

	return 0;
}
