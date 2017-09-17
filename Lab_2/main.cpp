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
	char errorBuffer[CURL_ERROR_SIZE];
	string html;
	if (GetHtml(&curl, sourceUrl, html, errorBuffer) != CURLE_OK)
	{
		PrintlnError(errorBuffer);
		return 1;
	}
	GetHrefs(html, [](const sregex_iterator & it) {
		cout << it->str() << "\n";
	});

	return 0;
}
