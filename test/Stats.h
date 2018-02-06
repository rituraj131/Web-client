#pragma once
#include "common.h"
class Stats
{
	std::queue<std::string> q;
	int val;
	int activeThreadCount;
public:
	Stats();
	void addURLToQueue(std::string url);
	std::string popURLFromQueue();
	void incrementVal(int count);
	int getVal();
	int getQueueSize();
	void changeThreadCount(int count);
	int getActiveThreadCount();
};

