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
	header2XX = 0;
	header3XX = 0;
	header4XX = 0;
	header5XX = 0;
	headerOthers = 0;
}

/*void Stats::addURLToQueue(std::string url) {
	q.push(url);
}

bool Stats::isURLQueueEmpty() {
	return q.empty();
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
}*/

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

void Stats::incrementHeaderCount(string header) {
	if (header.length() == 0)
		headerOthers++;
	else if (header.at(0) == '2')
		header2XX++;
	else if (header.at(0) == '2')
		header3XX++;
	else if (header.at(0) == '4')
		header4XX++;
	else if (header.at(0) == '5')
		header5XX++;
	else
		headerOthers++;
}

vector<int> Stats::getHeaderCount() {
	vector<int> list;
	list.push_back(header2XX);
	list.push_back(header3XX);
	list.push_back(header4XX);
	list.push_back(header5XX);
	list.push_back(headerOthers);
	return list;
}


