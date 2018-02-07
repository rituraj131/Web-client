#pragma once
#include "common.h"
class Stats
{
	std::queue<std::string> q;
	int activeThreadCount, extractedURLCount, uniqueHostCount, 
		dnsCount, uniqueIPCount, robotsPassedCount, crawledURLCount, linksCount;
	int header2XX, header3XX, header4XX, header5XX, headerOthers;
	long bytesRead;

public:
	Stats();
	/*void addURLToQueue(std::string);
	std::string popURLFromQueue();
	int getQueueSize();
	bool isURLQueueEmpty();*/
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
	void incrementHeaderCount(string);
	vector<int> getHeaderCount();
};

