#pragma once
#include "common.h"
#include "Stats.h"
#include "Urlparts.h"
#include "UrlValidator.h"
#include "Utility.h"
#include "PrevHost.h"
#include "MySocket.h"
#include "HTMLParserBase.h"
#define HTTP_STATUS_OK 200

class ThreadPool
{
public:
	void letTheGameBegin(Stats &, int, string);
};

