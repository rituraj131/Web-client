#include "common.h"
#include "UrlValidator.h"
#include "GetAndParse.h"
#include "Utility.h";
#include "StatsHandler.h"
#include "Stats.h"
#include "ThreadPool.h"

void statsThreadFunc(StatsHandler &, queue<string> &);
void crawlerThreadFunc(StatsHandler &, queue<string> &);

bool done = false;

int main(int argc, char **argv) {
	string threadStr(argv[1]), file_name(argv[2]);
	Utility utility;
	int thread_count = utility.getThreadCount(threadStr);
	Stats stats;
	ThreadPool tPool = ThreadPool(std::ref(stats), "list.txt");
	tPool.letTheGameBegin(std::ref(stats), thread_count);
}

int main111(int argc, char **argv)
{
	cout << "hello World! argc: " << argc << endl;
	if (argc == 2) {
		std::string url;
		cout << "Input URL: ";
		cin >> url;

		UrlValidator validate;
		UrlParts urlParts = validate.urlParser(url);

		if (urlParts.isValid == -10)//some failure in parsing observed!
			return 0;

		GetAndParse getAndParse;
		//getAndParse.getAndParseHTML(urlParts, true);
	}
	else if(argc == 3){
		int thread_count = 0;
		Utility utility;

		string threadStr(argv[1]), file_name(argv[2]);
	
		thread_count = utility.getThreadCount(threadStr);
		
		utility.validateFileExistence(file_name);

		//cout << "Opened " << file_name << " with size " << utility.filesize(file_name) << endl;

		string fileContent = utility.readFile(file_name);
		queue<string> urlListQueue = utility.split(fileContent, "\n");

		StatsHandler stats;

		thread statsThread(statsThreadFunc, ref(stats), ref(urlListQueue));
		cout << urlListQueue.size() << endl;

		
		thread *crawlerThreads = new thread[thread_count];

		for (int i = 0; i < thread_count; i++) {
			crawlerThreads[i] = thread(crawlerThreadFunc, ref(stats), ref(urlListQueue));
		}

		for (int i = 0; i < thread_count; i++) {
			crawlerThreads[i].join();
		}

		/*while (urlListQueue.size() != 0) {
			int tCount = 0;
			for (int i = 0; i < thread_count && i < urlListQueue.size(); i++) {
				string url = urlListQueue.front();
				urlListQueue.pop();
				tCount++;
				stats.incrementExtractedURLCount(1);
				crawlerThreads[i] = thread(crawlerThreadFunc, url, ref(stats));
				stats.incrementThreads(1);
			}

			for (int i = 0; i < tCount; i++) {
				crawlerThreads[i].join();
			}
		}*/
		done = true;
		statsThread.join();
	}
	else {
		cout << "Wrong input!" << endl;
	}
	cout << endl;
	return 1; 
}

void statsThreadFunc11(StatsHandler &stats, queue<string> &q) {
	int secCount = 0;
	mutex mtx;
	while (!done) {
		printf("[%3d] %l B %d T\n", secCount, stats.getBytes(), stats.getSuccessfullDNSCount());
		secCount += 2;
		cout << "Thread Count: " << stats.getActiveThreadCount() << endl;
		cout << "Bytes: " << stats.getBytes() << endl;
		cout << "CrawledURL Count: " << stats.getCrawledURLCount() << endl;
		mtx.lock();
		cout << "Queue Count: " << q.size() << endl;
		mtx.unlock();
		cout << "Robots Passed Count: " << stats.getRobotsPassedCount() << endl;
		cout << "UniqueHost Count: " << stats.getUniqueHostCount() << endl;
		Sleep(2000);
	}
}

void statsThreadFunc(StatsHandler &stats, queue<string> &q) {
	HANDLE hTimer = NULL;
	LARGE_INTEGER liDueTime;
	mutex mtx;

	liDueTime.QuadPart = -20000000LL;
	int secCount = 2;
	while (!done) {
		// Create an unnamed waitable timer.
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
			mtx.lock();
			printf("[%3d] %l B %d T\n", secCount, stats.getBytes(), stats.getSuccessfullDNSCount());
			secCount += 2;
			cout << "Thread Count: "<<stats.getActiveThreadCount() << endl;
			cout << "Bytes: "<<stats.getBytes() << endl;
			cout << "CrawledURL Count: "<<stats.getCrawledURLCount() << endl;
			cout << "Queue Count: " << q.size() << endl;
			cout << "Robots Passed Count: "<<stats.getRobotsPassedCount() << endl;
			cout << "UniqueHost Count: "<<stats.getUniqueHostCount() << endl;
			cout << "Unique IP count: " << stats.getUniqueIPCount() << endl;
			cout << "Extracted URL count: "<<stats.getExtractedURLCount() << endl;
			mtx.unlock();
		}
	}
}

void crawlerThreadFunc(StatsHandler &stats, queue<string> &urlListQueue) {
	GetAndParse getAndParse;
	UrlValidator validate;
	mutex mtx;

	while (true) {
		mtx.lock();
		if (urlListQueue.size() == 0) {
			mtx.unlock();
			break;
		}

		string url = urlListQueue.front();
		urlListQueue.pop();

		stats.incrementThreads(1);
		mtx.unlock();

		UrlParts urlParts = validate.urlParser(url);
		if (urlParts.isValid == -10) { //some failure in parsing observed!
			mtx.lock();
			stats.decrementThreads(1);
			mtx.unlock();
		}

		getAndParse.getAndParseHTML(urlParts, false, ref(stats));

		mtx.lock();
		stats.decrementThreads(1);
		mtx.unlock();
	}

	done = true;
	
}
