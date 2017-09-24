#include "stdafx.h"
#include "UrlParts.h"


CUrlParts::CUrlParts(const std::string & url)
{
	std::string urlCopy = url;
	if (CUrlParts::RemoveExtraSlashesInBeginning(urlCopy))
	{
		_isProtocolInherited = true;
	}
	if (!urlCopy.empty() && urlCopy[0] == '/')
	{
		_isAbsolutePath = true;
		urlCopy.erase(urlCopy.begin());
	}
	CUrlParts::ComputeFileName(urlCopy, _fileName, _protocol, _subDomainName, _domainName, _path, _isProtocolInherited);
}

std::string CUrlParts::GetProtocol() const
{
	return _protocol;
}

void CUrlParts::SetProtocol(const std::string & value)
{
	_protocol = value;
}

std::string CUrlParts::GetSubDomainName() const
{
	return _subDomainName;
}

void CUrlParts::SetSubDomainName(const std::string & value)
{
	_subDomainName = value;
}

std::string CUrlParts::GetDomainName() const
{
	return _domainName;
}

void CUrlParts::SetDomainName(const std::string & value)
{
	_domainName = value;
}

std::string CUrlParts::GetPath() const
{
	return _path;
}

void CUrlParts::SetPath(const std::string & value)
{
	_path = value;
}

std::string CUrlParts::GetFileName() const
{
	return _fileName;
}

void CUrlParts::SetFileName(const std::string & value)
{
	_fileName = value;
}

bool CUrlParts::IsAbsolutePath() const
{
	return _isAbsolutePath;
}

void CUrlParts::ComputeProtocol(const std::string & url, std::string & protocol)
{
	std::smatch match;
	std::regex_search(url, match, std::regex("^https?://"));
	protocol = match.str();
}

void CUrlParts::ComputeSubDomainName(const std::string & url, std::string & subDomainName, std::string & protocol, bool isProtocolInherited)
{
	CUrlParts::ComputeProtocol(url, protocol);
	if (protocol.empty() && !isProtocolInherited)
	{
		return;
	}
	std::smatch match;
	if (std::regex_search(url, match, std::regex("^" + protocol + "[^/.]+[.][^/.]+[.][^/.]+")))
	{
		subDomainName = url.substr(protocol.length());
		if (std::regex_search(subDomainName, match, std::regex("[^/.]+[.]")))
		{
			subDomainName = match.str();
			subDomainName.erase(--subDomainName.end());
		}
	}
}

void CUrlParts::ComputeDomainName(const std::string & url, std::string & domainName, std::string & protocol, std::string & subDomainName, bool isProtocolInherited)
{
	CUrlParts::ComputeSubDomainName(url, subDomainName, protocol, isProtocolInherited);
	if (protocol.empty() && !isProtocolInherited)
	{
		return;
	}

	std::string beforeDomainName = subDomainName.empty() ? protocol : protocol + subDomainName + ".";

	size_t escapedCharacterCount = 0;
	CUrlParts::EscapeCharacters(beforeDomainName, escapedCharacterCount);

	std::smatch match;
	if (std::regex_search(url, match, std::regex("^" + beforeDomainName + "[^/.]+[.][^/]+")))
	{
		domainName = match.str().substr(beforeDomainName.length() - escapedCharacterCount * 2);
		if (domainName[0] == '.')
		{
			domainName.erase(domainName.begin());
		}
	}
}

void CUrlParts::ComputePath(const std::string & url, std::string & path, std::string & protocol, std::string & subDomainName, std::string & domainName, bool isProtocolInherited)
{
	CUrlParts::ComputeDomainName(url, domainName, protocol, subDomainName, isProtocolInherited);

	std::string beforePath = protocol;
	if (!subDomainName.empty())
	{
		beforePath += subDomainName + ".";
	}
	if (!domainName.empty())
	{
		beforePath += domainName + "/";
	}
	size_t escapedCharacterCount = 0;
	CUrlParts::EscapeCharacters(beforePath, escapedCharacterCount);

	std::smatch match;
	if (std::regex_search(url, match, std::regex("^" + beforePath + ".*/")))
	{
		std::string matchStr = match.str();
		size_t beforePathLength = beforePath.length() - escapedCharacterCount * 2;
		path = matchStr.substr(beforePathLength, matchStr.length() - beforePathLength - 1);
		if (!path.empty() && path[0] == '/')
		{
			path.erase(path.begin());
		}
	}
}

void CUrlParts::ComputeFileName(const std::string & url, std::string & fileName, std::string & protocol, std::string & subDomainName, std::string & domainName, std::string & path, bool isProtocolInherited)
{
	CUrlParts::ComputePath(url, path, protocol, subDomainName, domainName, isProtocolInherited);

	std::string beforeFileName = protocol;
	if (!subDomainName.empty())
	{
		beforeFileName += subDomainName + ".";
	}
	if (!domainName.empty())
	{
		beforeFileName += domainName + "/";
	}
	if (!path.empty())
	{
		beforeFileName += path + "/";
	}
	size_t escapedCharacterCount = 0;
	CUrlParts::EscapeCharacters(beforeFileName, escapedCharacterCount);

	std::smatch match;
	if (std::regex_search(url, match, std::regex("^" + beforeFileName + ".*")))
	{
		size_t beforeFileNameLength = beforeFileName.length() - escapedCharacterCount * 2;
		fileName = match.str().substr(beforeFileNameLength);
	}
}

void CUrlParts::CreateFullUrl(const CUrlParts & urlParts, std::string & fullUrl)
{
	fullUrl = urlParts.GetProtocol();
	std::string subDomainName = urlParts.GetSubDomainName();
	if (!subDomainName.empty())
	{
		fullUrl += subDomainName + ".";
	}
	std::string domainName = urlParts.GetDomainName();
	if (!domainName.empty())
	{
		fullUrl += domainName + "/";
	}
	std::string path = urlParts.GetPath();
	if (!path.empty())
	{
		fullUrl += path + "/";
	}
	fullUrl += urlParts.GetFileName();
}

bool CUrlParts::RemoveExtraSlashesInBeginning(std::string & url)
{
	std::smatch match;
	if (std::regex_search(url, match, std::regex("^//[/]*")))
	{
		url.erase(url.begin(), url.begin() + match.length());
		return true;
	}
	return false;
}

void CUrlParts::EscapeCharacters(std::string & str, size_t & escapedCharacterCount)
{
	for (size_t i = 0; i < str.length(); ++i)
	{
		if (str[i] == '.')
		{
			str.insert(str.begin() + i, '[');
			str.insert(str.begin() + i + 2, ']');
			i += 2;
			++escapedCharacterCount;
		}
		if (str[i] == '+')
		{
			str.insert(str.begin() + i, '\\');
			++i;
			++escapedCharacterCount;
		}
	}
}
