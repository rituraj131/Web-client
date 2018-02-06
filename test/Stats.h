#pragma once
#include "common.h"
class Stats
{
	std::queue<std::string> q;
	int activeThreadCount, extractedURLCount, uniqueHostCount, 
		dnsCount, uniqueIPCount, robotsPassedCount, crawledURLCount, linksCount;
	long bytesRead;

public:
	Stats();
	void addURLToQueue(std::string);
	std::string popURLFromQueue();
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
	void incrementRobotsPassedCount();
	int getRobotsPassedCount();
	void incrementCrawledURLCount();
	int getCrawledURLCount();
	void incrementLinksCount(int);
	int getLinksCount();
};

