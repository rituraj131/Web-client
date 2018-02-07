#include "ThreadPool.h"

static mutex mtx;
static PrevHost prevHost;
static bool producerDone = false, crawlerDone = false;
static int threadCount = 0;
static std::condition_variable cv;
static std::queue<std::string> urlQueue;

void crawlerThreadFunc(Stats &);
void statsThreadFunc(Stats &);
void urlProducerThreadFunc(Stats &, string);

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

int getCrawledURLCount(Stats &stats) {
	mtx.lock();
	int count = stats.getCrawledURLCount();
	mtx.unlock();
	return count;
}

int getLinksCount(Stats &stats) {
	mtx.lock();
	int count = stats.getLinksCount();
	mtx.unlock();
	return count;
}

ThreadPool::ThreadPool()
{
	//producer(std::ref(stats), filename);
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

void ThreadPool::letTheGameBegin(Stats &stats, int thread_count, string filename) {
	threadCount = thread_count;
	std::thread urlProducerThread(urlProducerThreadFunc, ref(stats), filename);

	std::thread *crawlerThreads = new std::thread[thread_count];
	std::thread statsThread(statsThreadFunc, std::ref(stats));

	for (int i = 0; i < threadCount; i++) {
		crawlerThreads[i] = std::thread(crawlerThreadFunc, std::ref(stats));
	}

	if (urlProducerThread.joinable())
		urlProducerThread.join();

	for (int i = 0; i < threadCount; i++) {
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
		std::unique_lock<std::mutex> lk(mtx);
		if (urlQueue.empty()) {
			if (producerDone)
				break;
			continue;
		}
		cv.wait(lk, [] { return producerDone || !urlQueue.empty(); });

		std::string url = urlQueue.front();
		urlQueue.pop();
		lk.unlock();
		changeThreadCount(ref(stats), 1);
		if (url.compare("-1") == 0) {
			changeThreadCount(ref(stats), -1);
			if (!producerDone)
				continue;
			else
				break;
		}

		incrementURlExtractedCount(ref(stats));

		UrlParts urlParts = validate.urlParser(url);
		if (urlParts.isValid == -10) { //some failure in parsing observed!
			changeThreadCount(ref(stats), -1);	
			continue;
		}

		if (checkHostUniqueness(urlParts) == 0) {
			changeThreadCount(ref(stats), -1);
			continue;
		}

		incrementUniqueHostCount(ref(stats));

		Crawler crawler;
		crawler.crawl(ref(stats), urlParts);

		changeThreadCount(ref(stats), -1);
	}

	if(getActiveThreadCount(ref(stats)) == 0)
		crawlerDone = true;
}

void statsThreadFunc(Stats &stats) {
	HANDLE hTimer = NULL;
	LARGE_INTEGER liDueTime;

	liDueTime.QuadPart = -20000000LL;
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
			cout << "\nQueue Size: " << urlQueue.size() << endl;
			cout << "Extracted URL Count: " << getExtractedURLCount(ref(stats)) << endl;
			cout << "Active Threads: " << getActiveThreadCount(ref(stats)) << endl;
			cout << "Unique Host Count: " << getUniqueHostCount(ref(stats)) << endl;
			cout << "DNS count: " << getDNSCount(ref(stats)) << endl;
			cout<<"Unique IP Count: "<< getUniqueIPCount(ref(stats)) << endl;
			cout<<"Bytes Read: "<< getBytesRead(ref(stats)) << endl;
			cout << "Robots Test passed: " << getRobotsPassedCount(ref(stats)) << endl;
			cout<<"Total Crawled URL Count: "<< getCrawledURLCount(ref(stats)) << endl;
			cout<<"Total Links Found: "<< getLinksCount(ref(stats)) << endl;
		}
	}

	cout << "\n\nAll Done:" << endl;
	cout << "--------------" << endl;
	cout << "Queue Size: " << urlQueue.size() << endl;
	cout << "Extracted URL Count: " << getExtractedURLCount(ref(stats)) << endl;
	cout << "Active Threads: " << getActiveThreadCount(ref(stats)) << endl;
	cout << "Unique Host Count: " << getUniqueHostCount(ref(stats)) << endl;
	cout << "DNS count: " << getDNSCount(ref(stats)) << endl;
	cout << "Unique IP Count: " << getUniqueIPCount(ref(stats)) << endl;
	cout << "Bytes Read: " << getBytesRead(ref(stats)) << endl;
	cout << "Robots Test passed: " << getRobotsPassedCount(ref(stats)) << endl;
	cout << "Total Crawled URL Count: " << getCrawledURLCount(ref(stats)) << endl;
	cout << "Total Links Found: " << getLinksCount(ref(stats)) << endl;
}

void urlProducerThreadFunc(Stats &stats, string filename) {
	std::ifstream ifs(filename);
	if (!ifs) {
		producerDone = true;
		return;
	}
	
	Utility utility;
	bool fileFound = utility.validateFileExistence(filename);
	if (!fileFound) {
		producerDone = true;
		cout << "File not found!" << endl;
		return;
	}

	string url;
	while (std::getline(ifs, url)) {
		if (url.length() == 0) continue;
		
		{
			std::lock_guard<std::mutex> lk(mtx);
			urlQueue.push(url);
		}
		cv.notify_all();
	}
	{
		std::lock_guard<std::mutex> lk(mtx);
		producerDone = true;
	}
	cv.notify_all();
}