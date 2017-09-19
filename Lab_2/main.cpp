#include "stdafx.h"
#include "UTF8.h"
#include "UrlValidation.h"
#include "Print.h"
using namespace std;

const int ARG_COUNT = 2;

static int Writer(char *data, size_t size, size_t nmemb, string *buffer)
{
	int result = 0;
	if (buffer != NULL)
	{
		buffer->append(data, size * nmemb);
		result = size * nmemb;
	}

	return result;
}

CURLcode GetHtml(CURL ** curl, const string & sourceUrl, const string & html, char (&errorBuffer)[CURL_ERROR_SIZE])
{
	curl_easy_setopt(*curl, CURLOPT_ERRORBUFFER, errorBuffer);
	curl_easy_setopt(*curl, CURLOPT_URL, sourceUrl.c_str());
	curl_easy_setopt(*curl, CURLOPT_FOLLOWLOCATION, true);
	curl_easy_setopt(*curl, CURLOPT_HEADER, true);
	curl_easy_setopt(*curl, CURLOPT_WRITEFUNCTION, Writer);
	curl_easy_setopt(*curl, CURLOPT_WRITEDATA, &html);
	return curl_easy_perform(*curl);
}

void GetHrefs(const string & html, const function<void(const sregex_iterator &)> & callback)
{
	regex re("href=[\"'].+?[\"']");
	for (auto it = sregex_iterator(html.begin(), html.end(), re); it != sregex_iterator(); ++it)
	{
		callback(it);
	}
}

bool GetUrl(const string & href, string & url)
{
	bool result = false;
	smatch resultWithQuotes;
	if (regex_search(href, resultWithQuotes, regex("[\"'].+?[\"']")))
	{
		result = true;
	}
	url = resultWithQuotes.str();
	url.erase(url.begin());
	url.erase(--url.end());
	return result;
}

void CompleteFullURL(const string & sourceUrl, string & url)
{
	if (sourceUrl[sourceUrl.size() - 1] != '/' && url[0] != '/')
	{
		url = sourceUrl + '/' + url;
	}
	else
	{
		if (sourceUrl[sourceUrl.size() - 1] == '/' && url[0] == '/')
		{
			url.erase(url.begin());
		}
		url = sourceUrl + url;
	}
}

long GetResponseCode(CURL **curl)
{
	long responseCode;
	curl_easy_getinfo(*curl, CURLINFO_RESPONSE_CODE, &responseCode);

	return responseCode;
}

bool ProcessURL(CURL **curl, const string & sourceUrl, wofstream & urlsStatus, wofstream & badUrls)
{
	char errorBuffer[CURL_ERROR_SIZE];
	string html;
	if (GetHtml(curl, sourceUrl, html, errorBuffer) != CURLE_OK)
	{
		PrintlnError(to_string(GetResponseCode(curl)));
		PrintlnError(errorBuffer);
		return false;
	}
	Println(to_string(GetResponseCode(curl)), urlsStatus);

	vector<string> urls;
	GetHrefs(html, [&sourceUrl, &urls](const sregex_iterator & it) {
		string url;
		if (!GetUrl(it->str(), url) || DoesUrlContainsColonAndSlashesOnce(url))
		{
			return;
		}
		if (url.find(sourceUrl) == string::npos)
		{
			CompleteFullURL(sourceUrl, url);
		}
		urls.push_back(url);
	});
	for (const string & url : urls)
	{
		cout << url << "\n";
	}
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
	
	if (!ProcessURL(&curl, sourceUrl, urlsStatus, badUrls))
	{
		return 1;
	}

	return 0;
}
