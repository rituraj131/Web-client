#pragma once
#include "common.h"
#include "URLParts.h"
#include "UrlValidator.h"
#include "PrevHost.h"
#include "MySocket.h"
#include "HTMLParserBase.h"
#include "StatsHandler.h"
#define INITIAL_BUF_SIZE 8192
#define HTTP_STATUS_OK 200

class GetAndParse
{
public:
	void getAndParseHTML(UrlParts, bool, StatsHandler &);
};

