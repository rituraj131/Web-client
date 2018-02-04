#pragma once
#include "URLParts.h"
#include "UrlValidator.h"
#include "common.h"
#include "PrevHost.h"
#include "MySocket.h"
#include "HTMLParserBase.h"
#define INITIAL_BUF_SIZE 8192
#define HTTP_STATUS_OK 200
using namespace std;
class GetAndParse
{
public:
	void getAndParseHTML(UrlParts, bool);
};

