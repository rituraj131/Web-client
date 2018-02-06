#include "Stats.h"
Stats::Stats() {
	activeThreadCount = 0;
	extractedURLCount = 0;
	uniqueHostCount = 0;
	dnsCount = 0;
	uniqueIPCount = 0;
	bytesRead = 0;
	robotsPassedCount = 0;
	crawledURLCount = 0;
	linksCount = 0;
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

void Stats::incrementDNSCount() {
	dnsCount++;
}

int Stats::getDNSCount() {
	return dnsCount;
}

void Stats::incrementUniqueIPCount() {
	uniqueIPCount++;
}

int Stats::getUniqueIPCount() {
	return uniqueIPCount;
}

void Stats::incrementBytesRead(long bytesCount) {
	bytesRead += bytesCount;
}

long Stats::getBytesRead() {
	return bytesRead;
}

void Stats::incrementRobotsPassedCount() {
	robotsPassedCount++;
}

int Stats::getRobotsPassedCount() {
	return robotsPassedCount;
}

void Stats::incrementCrawledURLCount() {
	crawledURLCount++;
}

int Stats::getCrawledURLCount() {
	return crawledURLCount;
}

void Stats::incrementLinksCount(int count) {
	linksCount += count;
}

int Stats::getLinksCount() {
	return linksCount;
}


