#pragma once
#include "common.h"
#include "Stats.h"
#include "Urlparts.h"
#include "UrlValidator.h"
#include "Utility.h"
#include "PrevHost.h"
#include "Crawler.h"

class ThreadPool
{
public:
	ThreadPool();
	void letTheGameBegin(Stats &, int, string);
};

