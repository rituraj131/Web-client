#pragma once
#include "common.h"
#include "Stats.h"
#include "Urlparts.h"
#include "UrlValidator.h"
#include "Utility.h"

class ThreadPool
{
public:
	ThreadPool(Stats &, std::string);
	void letTheGameBegin(Stats &stats, int thread_count);
};

