#pragma once
#include "common.h"
class Stats
{
	std::queue<std::string> q;
	int val;
	int activeThreadCount, extractedURLCount, uniqueHostCount, 
		dnsCount, uniqueIPCount;
	long bytesRead;

public:
	Stats();
	void addURLToQueue(std::string);
	std::string popURLFromQueue();
	void incrementVal(int);
	int getVal();
	int getQueueSize();
	void changeThreadCount(int);
	int getActiveThreadCount();
	void incrementExtractedURLCount();
	int getExtractedURLCount();
	void incrementUniqueHostCount();
	int getUniqueHostCount();
	void incrementDNSCount();
	int getDNSCount();
	void incrementUniqueIPCount();
	int getUniqueIPCount();
	void incrementBytesRead(long);
	long getBytesRead();
};

