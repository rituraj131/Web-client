#pragma once
#include <atomic>

class StatsHandler
{
	std::atomic<int> active_threads, pendingQueue, extractedURLCount, uniqueHost, successfulDNSCount,
		uniqueIPCount, robotsPassedCount, crawledURLCount, totalLinks;
	std::atomic<long> bytes;
public:
	StatsHandler();
	~StatsHandler();
	void incrementThreads(int count);
	void decrementThreads(int count);
	void incrementQueue(int count);
	void decrementQueue(int count);
	void incrementExtractedURLCount(int count);
	void incrementUniqueHost(int count);
	void incrementDNSCount(int count);
	void incrementUniqueIPCount(int count);
	void incrementBytes(int count);
	void incrementCrawledURLCount(int count);
	void incrementTotalLinks(int count);
	void incrementExtractedURLCount(int count);

	int getActiveThreadCount();
	int getQueueCount();
	int getExtractedURLCount();
	int getUniqueHostCount();
	int getSuccessfullDNSCount();
	int getUniqueIPCount();
	int getRobotsPassedCount();
	int getCrawledURLCount();
	long getBytes();
};

