#pragma once
#include <fstream>
#include<string>
#include<iostream>
#include<vector>
#include "UrlParts.h"
#include "UrlValidator.h"
#ifdef _WIN32
#include <io.h> 
#define access    _access_s
#else
#include <unistd.h>
#endif
using namespace std;

class Utility
{
public:
	std::ifstream::pos_type filesize(std::string);
	int validateThreadCount(std::string);
	bool validateFileExistence(const std::string &Filename);
	std::string readFile(std::string file_name);
	vector<string> split(std::string s, string delimiter);
	void printURLList(vector<string> list);
	vector<UrlParts> getURLList(vector<string> list);
};

