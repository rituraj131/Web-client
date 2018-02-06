#pragma once
#include "common.h"
class UrlParts {
public:
	std::string url;
	int port_no;
	std::string host, scheme, query, path;
	int isValid;
};

