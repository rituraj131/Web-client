#include "Stats.h"
Stats::Stats() {
	val = 0;
	activeThreadCount = 0;
	extractedURLCount = 0;
	uniqueHostCount = 0;
}

void Stats::addURLToQueue(std::string url) {
	q.push(url);
}

std::string Stats::popURLFromQueue() {
	if (q.size() == 0)
		return "-1";
	std::string url = q.front();
	q.pop();
	return url;
}

void Stats::incrementVal(int count) {
	val += count;
}

int Stats::getVal() {
	return val;
}

int Stats::getQueueSize() {
	return q.size();
}

void Stats::changeThreadCount(int count) {
	activeThreadCount += count;
}

int Stats::getActiveThreadCount() {
	return activeThreadCount;
}

void Stats::incrementExtractedURLCount() {
	extractedURLCount += 1;
}

int Stats::getExtractedURLCount() {
	return extractedURLCount;
}

void Stats::incrementUniqueHostCount() {
	uniqueHostCount++;
}

int Stats::getUniqueHostCount() {
	return uniqueHostCount;
}


