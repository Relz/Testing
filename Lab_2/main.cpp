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
	CURL *curl = curl_easy_init();
	CURLcode result;

	if (curl)
	{
		char errorBuffer[CURL_ERROR_SIZE];
		string buffer;
		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, true);
		curl_easy_setopt(curl, CURLOPT_HEADER, true);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Writer);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
		result = curl_easy_perform(curl);
		if (result == CURLE_OK)
		{
			cout << buffer;
		}
		else
		{
			PrintlnError(errorBuffer);
		}
	}

	return 0;
}
