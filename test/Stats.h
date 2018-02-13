#pragma once
#include "common.h"
class Stats
{
	std::atomic<int> activeThreadCount, extractedURLCount, uniqueHostCount,
		dnsCount, uniqueIPCount, robotsPassedCount, crawledURLCount, linksCount;
	std::atomic<int> tamuHostCount, tamuHostInsideCount;
	std::atomic<int> header2XX, header3XX, header4XX, header5XX, headerOthers;
	std::atomic<long> bytesRead;

public:
	Stats();
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
	void incremenTamuHostCount(int);
	int getTamuHostCount();
	void incremenTamuHostInsideCount(int count);
	int getTamuHostInsideCount();
};

