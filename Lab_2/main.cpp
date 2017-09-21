#include "stdafx.h"
#include "UTF8.h"
#include "UrlValidation.h"
#include "Print.h"
using namespace std;

const int ARG_COUNT = 2;

struct HrefUrlParts
{
	string path;
	string fileName;
};

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

bool GetFileName(const string & url, string & fileName)
{
	bool result = false;
	smatch fileNameMatch;
	if (regex_search(url, fileNameMatch, regex("[^/]+$")))
	{
		result = true;
	}
	fileName = fileNameMatch.str();
	if (fileName[0] == '/')
	{
		fileName.erase(fileName.begin());
	}
	return result;
}

bool GetPath(const string & url, string & path)
{
	bool result = false;
	smatch pathMatch;
	if (regex_search(url, pathMatch, regex("(\$/.*?/)[^/]*?\.\S*")))
	{
		result = true;
	}
	path = pathMatch.str();
	if (path[0] != '/')
	{
		path.insert(path.begin(), '/');
	}
	if (path[path.size() - 1] != '/')
	{
		path.push_back('/');
	}
	return result;
}

void CompleteFullURL(const string & sourceUrl, const string & currentUrl, string & url)
{
	const string & rootUrl = (url[0] == '/') ? sourceUrl : currentUrl;
	if (rootUrl[rootUrl.size() - 1] != '/' && url[0] != '/')
	{
		url = rootUrl + '/' + url;
	}
	else
	{
		if (rootUrl[rootUrl.size() - 1] == '/' && url[0] == '/')
		{
			url.erase(url.begin());
		}
		url = rootUrl + url;
	}
}

void GetHrefUrlParts(const string & url, HrefUrlParts & hrefUrlParts)
{
	GetFileName(url, hrefUrlParts.fileName);
	GetPath(url, hrefUrlParts.path);
}

long GetResponseCode(CURL **curl)
{
	long responseCode;
	curl_easy_getinfo(*curl, CURLINFO_RESPONSE_CODE, &responseCode);

	return responseCode;
}

bool ProcessURL(
	CURL **curl,
	const string & sourceUrl,
	const HrefUrlParts & hrefUrlParts,
	wofstream & urlsStatus,
	wofstream & badUrls,
	queue<HrefUrlParts> & queue,
	unordered_set<string> & processedUrls)
{
	string currentUrl = sourceUrl + hrefUrlParts.path + hrefUrlParts.fileName;
	Println(u8"----------");
	Println(currentUrl);
	Println(u8"Получение HTML кода страницы");
	char errorBuffer[CURL_ERROR_SIZE];
	string html;
	if (GetHtml(curl, currentUrl, html, errorBuffer) != CURLE_OK)
	{
		PrintlnError(errorBuffer);
		return false;
	}
	long responseCode = GetResponseCode(curl);
	if (responseCode != 200)
	{
		Println(currentUrl + u8" : " + to_string(responseCode), badUrls);
	}
	Println(currentUrl + u8" : " + to_string(responseCode), urlsStatus);

	Println(u8"Получение внутренних ссылок страницы");
	GetHrefs(html, [&sourceUrl, &currentUrl, &processedUrls, &queue](const sregex_iterator & it) {
		string url;
		if (!GetUrl(it->str(), url) || DoesUrlContainsColonAndSlashesOnce(url))
		{
			return;
		}
		HrefUrlParts hrefUrlParts;
		GetHrefUrlParts(url, hrefUrlParts);
		url = sourceUrl + hrefUrlParts.path + hrefUrlParts.fileName;
		if (processedUrls.find(url) == processedUrls.end())
		{
			processedUrls.insert(url);
			queue.push(hrefUrlParts);
		}
	});
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
	if (sourceUrl[sourceUrl.size() - 1] == '/')
	{
		sourceUrl.erase(--sourceUrl.end());
	}
	CURL *curl = curl_easy_init();

	if (curl == nullptr)
	{
		PrintlnError(u8"Не удалось инициализировать библиотеку для работы с сетью");
		return 1;
	}
	wofstream urlsStatus("urls_status.txt");
	wofstream badUrls("bad_urls.txt");
	
	HrefUrlParts hrefSourceUrlParts;
	hrefSourceUrlParts.path = "/";

	unordered_set<string> processedUrls;
	processedUrls.insert(sourceUrl + hrefSourceUrlParts.path + hrefSourceUrlParts.fileName);

	queue<HrefUrlParts> queue;
	queue.push(hrefSourceUrlParts);
	while (!queue.empty())
	{
		HrefUrlParts currentHrefUrlParts = queue.front();
		queue.pop();
		ProcessURL(&curl, sourceUrl, currentHrefUrlParts, urlsStatus, badUrls, queue, processedUrls);
	}

	return 0;
}
