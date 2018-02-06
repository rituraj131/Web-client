#include "ThreadPool.h"

static mutex mtx;
static PrevHost prevHost;
static bool producerDone = false, crawlerDone = false;

void crawlerThreadFunc(Stats &);
void statsThreadFunc(Stats &);

void changeThreadCount(Stats &stats, int count) {
	mtx.lock();
	stats.changeThreadCount(count);
	mtx.unlock();
}

int getActiveThreadCount(Stats &stats) {
	mtx.lock();
	int count = stats.getActiveThreadCount();
	mtx.unlock();
	return count;
}

void incrementURlExtractedCount(Stats &stats) {
	mtx.lock();
	stats.incrementExtractedURLCount();
	mtx.unlock();
}

int getExtractedURLCount(Stats &stats) {
	mtx.lock();
	int count = stats.getExtractedURLCount();
	mtx.unlock();
	return count;
}

void incrementUniqueHostCount(Stats &stats) {
	mtx.lock();
	stats.incrementUniqueHostCount();
	mtx.unlock();
}

int getUniqueHostCount(Stats &stats) {
	mtx.lock();
	int count = stats.getUniqueHostCount();
	mtx.unlock();
	return count;
}

int getDNSCount(Stats &stats) {
	mtx.lock();
	int count = stats.getDNSCount();
	mtx.unlock();
	return count;
}

int getUniqueIPCount(Stats &stats) {
	mtx.lock();
	int count = stats.getUniqueIPCount();
	mtx.unlock();
	return count;
}

long getBytesRead(Stats &stats) {
	mtx.lock();
	long count = stats.getBytesRead();
	mtx.unlock();
	return count;
}

int getRobotsPassedCount(Stats &stats) {
	mtx.lock();
	int count = stats.getRobotsPassedCount();
	mtx.unlock();
	return count;
}

void producer(Stats &stats, std::string filename) {
	std::ifstream ifs(filename);
	std::string url;
	while (std::getline(ifs, url)) {
		if (url.length() == 0) continue;

		mtx.lock();
		stats.addURLToQueue(url);
		mtx.unlock();
	}
	producerDone = true;
}

std::string consumer(Stats &stats) {
	mtx.lock();
	std::string url = stats.popURLFromQueue();
	mtx.unlock();
	return url;
}

ThreadPool::ThreadPool(Stats &stats, std::string filename)
{
	producer(std::ref(stats), filename);
}

bool checkHostUniqueness(UrlParts urlParts) {
	mtx.lock();
	bool ret = true;
	if (prevHost.checkIfHostUnique(urlParts.host) == 0)
		ret = false;
	mtx.unlock();
	return ret;
}

bool checkIPUniqueness(char *address) {
	mtx.lock();
	bool ret = true;
	if (prevHost.checkIfIPUnique(address) == 0)
		ret = false;
	mtx.unlock();
	return ret;
}

void ThreadPool::letTheGameBegin(Stats &stats, int thread_count) {
	std::thread *crawlerThreads = new std::thread[thread_count];
	std::thread statsThread(statsThreadFunc, std::ref(stats));

	for (int i = 0; i < thread_count; i++) {
		crawlerThreads[i] = std::thread(crawlerThreadFunc, std::ref(stats));
	}

	for (int i = 0; i < thread_count; i++) {
		if (crawlerThreads[i].joinable())
			crawlerThreads[i].join();
	}

	if (statsThread.joinable())
		statsThread.join();
}

void crawlerThreadFunc(Stats &stats) {
	UrlValidator validate;
	Utility utility;
	while (true) {
		std::string url = consumer(std::ref(stats));

		if (url.compare("-1") == 0) {
			if (!producerDone)
				continue;
			else
				break;
		}
		incrementURlExtractedCount(ref(stats));
		changeThreadCount(ref(stats), 1);

		UrlParts urlParts = validate.urlParser(url);
		if (urlParts.isValid == -10) { //some failure in parsing observed!
			mtx.lock();
			changeThreadCount(ref(stats), -1);
			mtx.unlock();
			continue;
		}

		if (checkHostUniqueness(urlParts) == 0)
			continue;

		incrementUniqueHostCount(ref(stats));

		Crawler crawler;
		crawler.crawl(ref(stats), urlParts);

		Sleep(500);
		changeThreadCount(ref(stats), -1);
	}
	crawlerDone = true;
}

void statsThreadFunc(Stats &stats) {
	HANDLE hTimer = NULL;
	LARGE_INTEGER liDueTime;

	liDueTime.QuadPart = -10000000LL;
	int secCount = 2;
	
	while (!producerDone || !crawlerDone) {
		hTimer = CreateWaitableTimer(NULL, TRUE, NULL);
		if (NULL == hTimer)
		{
			printf("CreateWaitableTimer failed (%d)\n", GetLastError());
			return;
		}

		// Set a timer to wait for 10 seconds.
		if (!SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0))
		{
			printf("SetWaitableTimer failed (%d)\n", GetLastError());
			return;
		}

		// Wait for the timer.
		if (WaitForSingleObject(hTimer, INFINITE) != WAIT_OBJECT_0)
			printf("WaitForSingleObject failed (%d)\n", GetLastError());
		else {
			cout << "\nQueue Size: " << stats.getQueueSize() << endl;
			cout << "Extracted URL Count: " << getExtractedURLCount(ref(stats)) << endl;
			cout << "Active Threads: " << getActiveThreadCount(ref(stats)) << endl;
			cout << "Unique Host Count: " << getUniqueHostCount(ref(stats)) << endl;
			cout << "DNS count: " << getDNSCount(ref(stats)) << endl;
			cout<<"Unique IP Count: "<< getUniqueIPCount(ref(stats)) << endl;
			cout<<"Bytes Read: "<< getBytesRead(ref(stats)) << endl;
			cout << "Robots Test passed: " << getRobotsPassedCount(ref(stats)) << endl;
		}
	}

	cout << "\n\nAll Done:" << endl;
	cout << "--------------" << endl;
	cout << "Queue Size: " << stats.getQueueSize() << endl;
	cout << "Extracted URL Count: " << getExtractedURLCount(ref(stats)) << endl;
	cout << "Active Threads: " << getActiveThreadCount(ref(stats)) << endl;
	cout << "Unique Host Count: " << getUniqueHostCount(ref(stats)) << endl;
	cout << "DNS count: " << getDNSCount(ref(stats)) << endl;
	cout << "Unique IP Count: " << getUniqueIPCount(ref(stats)) << endl;
	cout << "Bytes Read: " << getBytesRead(ref(stats)) << endl;
	cout << "Robots Test passed: " << getRobotsPassedCount(ref(stats)) << endl;
}