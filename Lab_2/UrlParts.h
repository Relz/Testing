#pragma once
class CUrlParts
{
public:
	CUrlParts(const std::string & url);

	std::string GetProtocol() const;
	void SetProtocol(const std::string & value);
	std::string GetSubDomainName() const;
	void SetSubDomainName(const std::string & value);
	std::string GetDomainName() const;
	void SetDomainName(const std::string & value);
	std::string GetPath() const;
	void SetPath(const std::string & value);
	std::string GetFileName() const;
	void SetFileName(const std::string & value);
	bool IsAbsolutePath() const;

	static void ComputeProtocol(const std::string & url, std::string & protocol);
	static void ComputeSubDomainName(const std::string & url, std::string & subDomainName, std::string & protocol = std::string(), bool isProtocolInherited = false);
	static void ComputeDomainName(const std::string & url, std::string & domainName, std::string & protocol = std::string(), std::string & subDomainName = std::string(), bool isProtocolInherited = false);
	static void ComputePath(const std::string & url, std::string & path, std::string & protocol = std::string(), std::string & subDomainName = std::string(), std::string & domainName = std::string(), bool isProtocolInherited = false);
	static void ComputeFileName(const std::string & url, std::string & fileName, std::string & protocol = std::string(), std::string & subDomainName = std::string(), std::string & domainName = std::string(), std::string & path = std::string(), bool isProtocolInherited = false);
	static void CreateFullUrl(const CUrlParts & urlParts, std::string & fullUrl);

private:
	static bool RemoveExtraSlashesInBeginning(std::string & url);
	static void EscapeCharacters(std::string & str, size_t & escapedCharacterCount);

	std::string _protocol;
	std::string _subDomainName;
	std::string _domainName;
	std::string _path;
	std::string _fileName;

	bool _isAbsolutePath = false;
	bool _isProtocolInherited = false;
};
