#include "ThreadPool.h"

static mutex mtx;
static bool producerDone = false, crawlerDone = false;

int getRandomNo();
void statsThreadFunc(Stats &);
void crawlerThreadFunc(Stats &);
void changeThreadCount(Stats &, int);
int getActiveThreadCount(Stats &);
void incrementURlExtractedCount(Stats &);
int getExtractedURLCount(Stats &);

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

		Sleep(500);
		stats.incrementVal(getRandomNo());
		changeThreadCount(ref(stats), -1);
	}
	crawlerDone = true;
}

void statsThreadFunc(Stats &stats) {
	HANDLE hTimer = NULL;
	LARGE_INTEGER liDueTime;

	liDueTime.QuadPart = -10000000LL;
	int secCount = 2;
	cout << "Queue to start with: " << stats.getQueueSize() << endl;
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
			cout << "\nVal: " << stats.getVal() << endl;
			cout << "Queue Size: " << stats.getQueueSize() << endl;
			cout << "Extracted URL Count: " << getExtractedURLCount(ref(stats)) << endl;
			cout << "Active Threads: " << getActiveThreadCount(ref(stats)) << endl;
		}
	}
}

int getRandomNo() {
	return std::rand() % 10;
}

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