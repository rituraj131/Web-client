#include "StatsHandler.h"



StatsHandler::StatsHandler()
{
	active_threads = 0;
	pendingQueue = 0; 
	extractedURLCount = 0; 
	uniqueHost = 0; 
	successfulDNSCount = 0; 
	uniqueIPCount = 0; 
	robotsPassedCount = 0; 
	crawledURLCount = 0; 
	totalLinks = 0;
	bytes = 0;
}


StatsHandler::~StatsHandler()
{
}

void StatsHandler::incrementThreads(int count) {
	active_threads += count;
}

void StatsHandler::decrementThreads(int count) {
	active_threads -= count;
}

void StatsHandler::incrementQueue(int count) {
	pendingQueue += count;
}

void StatsHandler::decrementQueue(int count) {
	pendingQueue += count;
}

void StatsHandler::incrementExtractedURLCount(int count) {
	extractedURLCount += count;
}

void StatsHandler::incrementUniqueHost(int count) {
	uniqueHost += count;
}

void StatsHandler::incrementDNSCount(int count) {
	successfulDNSCount += count;
}

void StatsHandler::incrementUniqueIPCount(int count) {
	uniqueIPCount += count;
}

void StatsHandler::incrementBytes(int count) {
	bytes += count;
}

void StatsHandler::incrementCrawledURLCount(int count) {
	crawledURLCount += count;
}

void StatsHandler::incrementTotalLinks(int count) {
	totalLinks += count;
}

void StatsHandler::incrementExtractedURLCount(int count) {
	extractedURLCount += count;
}

int StatsHandler::getActiveThreadCount() {
	return active_threads;
}

int StatsHandler::getQueueCount() {
	return pendingQueue;
}

int StatsHandler::getExtractedURLCount() {
	return extractedURLCount;
}

int StatsHandler::getUniqueHostCount() {
	return uniqueHost;
}

int StatsHandler::getSuccessfullDNSCount() {
	return successfulDNSCount;
}

int StatsHandler::getUniqueIPCount() {
	return uniqueIPCount;
}

int StatsHandler::getRobotsPassedCount() {
	return robotsPassedCount;
}

int StatsHandler::getCrawledURLCount() {
	return crawledURLCount;
}

long StatsHandler::getBytes() {
	return bytes;
}

