#pragma once
#include "common.h"
#include "UrlParts.h"
#include "UrlValidator.h"
#ifdef _WIN32
#include <io.h> 
#define access    _access_s
#else
#include <unistd.h>
#endif

class Utility
{
public:
	std::ifstream::pos_type filesize(std::string);
	int validateThreadCount(std::string);
	bool validateFileExistence(const std::string &Filename);
	std::string readFile(std::string file_name);
	queue<string> split(std::string s, string delimiter);
	void printURLList(vector<string> list);
	int getThreadCount(std::string count);
	void printData(UrlParts urlParts);
	bool Utility::isItChunked(string);
	string dechunnkedBody(string body);
	bool Utility::urlTAMUChecker(string);
};

