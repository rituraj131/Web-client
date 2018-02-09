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
	std::lock_guard<std::mutex> lk(mtx);
	stats.changeThreadCount(count);
	//mtx.unlock();
}

int getActiveThreadCount(Stats &stats) {
	std::lock_guard<std::mutex> lk(mtx);
	int count = stats.getActiveThreadCount();
	//mtx.unlock();
	return count;
}

void incrementURlExtractedCount(Stats &stats) {
	std::lock_guard<std::mutex> lk(mtx);
	stats.incrementExtractedURLCount();
	//mtx.unlock();
}

int getExtractedURLCount(Stats &stats) {
	std::lock_guard<std::mutex> lk(mtx);
	int count = stats.getExtractedURLCount();
	//mtx.unlock();
	return count;
}

void incrementUniqueHostCount(Stats &stats) {
	std::lock_guard<std::mutex> lk(mtx);
	stats.incrementUniqueHostCount();
	//mtx.unlock();
}

int getUniqueHostCount(Stats &stats) {
	std::lock_guard<std::mutex> lk(mtx);
	int count = stats.getUniqueHostCount();
	//mtx.unlock();
	return count;
}

int getDNSCount(Stats &stats) {
	std::lock_guard<std::mutex> lk(mtx);
	int count = stats.getDNSCount();
	//mtx.unlock();
	return count;
}

int getUniqueIPCount(Stats &stats) {
	std::lock_guard<std::mutex> lk(mtx);
	int count = stats.getUniqueIPCount();
	//mtx.unlock();
	return count;
}

long getBytesRead(Stats &stats) {
	std::lock_guard<std::mutex> lk(mtx);
	long count = stats.getBytesRead();
	//mtx.unlock();
	return count;
}

int getRobotsPassedCount(Stats &stats) {
	std::lock_guard<std::mutex> lk(mtx);
	int count = stats.getRobotsPassedCount();
	//mtx.unlock();
	return count;
}

int getCrawledURLCount(Stats &stats) {
	std::lock_guard<std::mutex> lk(mtx);
	int count = stats.getCrawledURLCount();
	//mtx.unlock();
	return count;
}

int getLinksCount(Stats &stats) {
	std::lock_guard<std::mutex> lk(mtx);
	int count = stats.getLinksCount();
	//mtx.unlock();
	return count;
}

ThreadPool::ThreadPool()
{
	//producer(std::ref(stats), filename);
}

bool checkHostUniqueness(UrlParts urlParts) {
	std::lock_guard<std::mutex> lk(mtx);
	bool ret = true;
	if (prevHost.checkIfHostUnique(urlParts.host) == 0)
		ret = false;
	//mtx.unlock();
	return ret;
}

bool checkIPUniqueness(char *address) {
	std::lock_guard<std::mutex> lk(mtx);
	bool ret = true;
	if (prevHost.checkIfIPUnique(address) == 0)
		ret = false;
	//mtx.unlock();
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
	long lastDownloadSize = 0, lastPagesCount = 0;
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
			printf("[%3d] %5d Q %6d E %7d H %6d D %6d I %5d R %5d C %5d L %4d\n",
				secCount, getActiveThreadCount(ref(stats)), urlQueue.size(), 
				getExtractedURLCount(ref(stats)), getUniqueHostCount(ref(stats)),
				getDNSCount(ref(stats)), getUniqueIPCount(ref(stats)), 
				getRobotsPassedCount(ref(stats)), getCrawledURLCount(ref(stats)),
				getLinksCount(ref(stats)));
			
			long tempBytesCount = getBytesRead(ref(stats));
			float dataThisTime = (((float)tempBytesCount - (float)(lastDownloadSize)) * 8.0)/(2*1000000);
			lastDownloadSize = tempBytesCount;
			int tempRobotsPassedCount = getRobotsPassedCount(ref(stats));
			int thisPagesCount = tempRobotsPassedCount - lastPagesCount;
			lastPagesCount = tempRobotsPassedCount;
			
			cout << "      *** crawling " << thisPagesCount / 2 << " pps @ " << 
				std::setprecision(2)<<dataThisTime << " Mbps\n\n";
			//printf("      *** crawling %.1f pps @ %.2f Mbps\n\n", thisPagesCount/2, (dataThisTime));
			secCount += 2;
		}
	}
	secCount -= 2;
	cout << endl;
	cout << "Final Report" << endl;
	cout << "---------------" << endl;
	printf("Extracted %7d URLs @ %.1f/s\n", getExtractedURLCount(ref(stats)), 
		((float)getExtractedURLCount(ref(stats))/secCount));
	printf("Looked up %7d DNS names @ %.1f/s\n", getDNSCount(ref(stats)), 
		((float)getDNSCount(ref(stats))/secCount));
	printf("Downloaded %7d robots @ %.1f/s\n", getRobotsPassedCount(ref(stats)),
		((float)getRobotsPassedCount(ref(stats))/secCount));
	printf("Crawled %7d pages @ %.1f/s\n", getCrawledURLCount(ref(stats)),
		((float)getCrawledURLCount(ref(stats))/secCount));
	printf("Parsed %7d links @ %.1f/s\n", getLinksCount(ref(stats)),
		((float)getLinksCount(ref(stats))/secCount));
	vector<int> headerList = stats.getHeaderCount();
	printf("HTTP codes: 2xx = %5d, 3xx = %5d, 4xx = %5d, 5xx = %5d, other = %5d\n",
		headerList.at(0), headerList.at(1), headerList.at(2), headerList.at(3), headerList.at(4));
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