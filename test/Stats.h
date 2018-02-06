#pragma once
#include "common.h"
class Stats
{
	std::queue<std::string> q;
	int val;
	int activeThreadCount, extractedURLCount;
public:
	Stats();
	void addURLToQueue(std::string);
	std::string popURLFromQueue();
	void incrementVal(int);
	int getVal();
	int getQueueSize();
	void changeThreadCount(int);
	int getActiveThreadCount();
	void incrementExtractedURLCount(int);
	int getExtractedURLCount();
};

