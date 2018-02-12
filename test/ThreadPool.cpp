#include "ThreadPool.h"

std::mutex mtx[20];
PrevHost prevHost;
Stats stats;
static bool producerDone = false, crawlerDone = false;
static int threadCount = 0;
static std::condition_variable cv;
static std::queue<std::string> urlQueue;

void crawlerThreadFunc();
void statsThreadFunc();
void urlProducerThreadFunc(string);
void crawlMyPage(UrlParts, HTMLParserBase *);
void printCurrStatistics(int, int, float);
void printFinalStatistics(int);

void incrementDNSCount() {
	std::lock_guard<std::mutex> lk(mtx[0]);
	stats.incrementDNSCount();
}

void incrementUniqueIPCount() {
	std::lock_guard<std::mutex> lk(mtx[1]);
	stats.incrementUniqueIPCount();
}

void incrementBytesRead(long bytesRead) {
	std::lock_guard<std::mutex> lk(mtx[2]);
	stats.incrementBytesRead(bytesRead);
}

void incrementRobotsPassedCount() {
	std::lock_guard<std::mutex> lk(mtx[3]);
	stats.incrementRobotsPassedCount();
}

void incrementCrawledURLCount() {
	std::lock_guard<std::mutex> lk(mtx[4]);
	stats.incrementCrawledURLCount();
}

void incrementLinksCount(int count) {
	std::lock_guard<std::mutex> lk(mtx[5]);
	stats.incrementLinksCount(count);
}

void incrementHeaderCount(string header) {
	std::lock_guard<std::mutex> lk(mtx[6]);
	stats.incrementHeaderCount(header);
}

void changeThreadCount(int count) {
	std::lock_guard<std::mutex> lk(mtx[7]);
	stats.changeThreadCount(count);
}

int getActiveThreadCount() {
	std::lock_guard<std::mutex> lk(mtx[8]);
	int count = stats.getActiveThreadCount();
	return count;
}

void incrementURlExtractedCount() {
	std::lock_guard<std::mutex> lk(mtx[9]);
	stats.incrementExtractedURLCount();
}

void incrementUniqueHostCount() {
	std::lock_guard<std::mutex> lk(mtx[10]);
	stats.incrementUniqueHostCount();
}

int getLinkCountInThePage(char *fileBuf, char *baseUrl)
{
	std::lock_guard<std::mutex> lk(mtx[16]);
	// create new parser object
	HTMLParserBase *parser = new HTMLParserBase;

	int nLinks;
	char *linkBuffer = parser->Parse(fileBuf, strlen(fileBuf), baseUrl, (int)strlen(baseUrl), &nLinks);

	// check for errors indicated by negative values
	if (nLinks < 0) { nLinks = 0; }

	delete parser;		// this internally deletes linkBuffer
	return nLinks;
}

bool checkHostUniqueness(UrlParts urlParts) {
	std::lock_guard<std::mutex> lk(mtx[10]);
	bool ret = true;
	if (prevHost.checkIfHostUnique(urlParts.host) == 0)
		ret = false;
	return ret;
}

bool checkIPUniqueness(char *address) {
	std::lock_guard<std::mutex> lk(mtx[11]);
	bool ret = true;
	if (prevHost.checkIfIPUnique(address) == 0)
		ret = false;
	return ret;
}

void ThreadPool::letTheGameBegin(int thread_count, string filename) {
	threadCount = thread_count;
	std::thread urlProducerThread(urlProducerThreadFunc, filename);

	std::thread *crawlerThreads = new std::thread[thread_count];
	std::thread statsThread(statsThreadFunc);

	for (int i = 0; i < threadCount; i++) {
		crawlerThreads[i] = std::thread(crawlerThreadFunc);
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

void crawlerThreadFunc() {
	UrlValidator validate;
	Utility utility;
	changeThreadCount(1);
	string url;
	HTMLParserBase *parser = new HTMLParserBase;

	while (true) {
		{
			std::unique_lock<std::mutex> lk(mtx[18]);
			if (urlQueue.empty()) {
				if (producerDone)
					break;
				continue;
			}

			cv.wait(lk, [] { return producerDone || !urlQueue.empty(); });

			url = urlQueue.front();
			urlQueue.pop();
		}

		incrementURlExtractedCount();

		UrlParts urlParts = validate.urlParser(url, false);
		if (urlParts.isValid == -10)
			continue;

		if (checkHostUniqueness(urlParts) == 0)
			continue;

		incrementUniqueHostCount();

		crawlMyPage(urlParts, parser);
	}

	changeThreadCount(-1);
	delete(parser);
	if(getActiveThreadCount() == 0)
		crawlerDone = true;
}

void statsThreadFunc() {
	HANDLE hTimer = NULL;
	LARGE_INTEGER liDueTime;

	liDueTime.QuadPart = -20000000LL;
	int secCount = 2;
	long lastDownloadSize = 0;
	int lastPagesCount = 0;
	while (!producerDone || !crawlerDone) {
		/*hTimer = CreateWaitableTimer(NULL, TRUE, NULL);
		if (NULL == hTimer)
		{
			std::printf("CreateWaitableTimer failed (%d)\n", GetLastError());
			return;
		}

		if (!SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0))
		{
			std::printf("SetWaitableTimer failed (%d)\n", GetLastError());
			return;
		}
		 
		if (WaitForSingleObject(hTimer, INFINITE) != WAIT_OBJECT_0)
			std::printf("WaitForSingleObject failed (%d)\n", GetLastError());
		else {*/
		Sleep(2000);
			long tempBytesCount = stats.getBytesRead();
			float dataThisTime = (((float)tempBytesCount - (float)(lastDownloadSize)) * 8.0)/(2*1000000);
			lastDownloadSize = tempBytesCount;
			int tempRobotsPassedCount = stats.getRobotsPassedCount();
			int thisPagesCount = tempRobotsPassedCount - lastPagesCount;
			lastPagesCount = tempRobotsPassedCount;
			
			printCurrStatistics(secCount, thisPagesCount/2, dataThisTime);
			
			secCount += 2;
		//}
	}
	secCount -= 2;
	printFinalStatistics(secCount);
}

void urlProducerThreadFunc(string filename) {
	std::ifstream ifs(filename);
	if (!ifs) {
		producerDone = true;
		return;
	}
	
	Utility utility;
	bool fileFound = utility.validateFileExistence(filename);
	if (!fileFound) {
		producerDone = true;
		std::cout << "File not found!" << endl;
		return;
	}

	string url;
	while (std::getline(ifs, url)) {
		if (url.length() == 0) continue;
		
		{
			std::lock_guard<std::mutex> lk(mtx[18]);
			urlQueue.push(url);
		}
		cv.notify_all();
	}
	{
		std::lock_guard<std::mutex> lk(mtx[18]);
		producerDone = true;
	}
	cv.notify_all();
}

bool finishMyCrawl(Socket socket, UrlParts urlParts, bool isRobot, struct sockaddr_in server, HTMLParserBase *parser) {
	if (!socket.socket_connect(server))
		return false;

	char *req_body = new char[INITIAL_BUF_SIZE];

	std::string req = "/";
	if (urlParts.path.size() > 0 || urlParts.query.size() > 0) {
		if (urlParts.path.size() > 0)
			req += urlParts.path;
		if (urlParts.query.size() > 0)
			req += "?" + urlParts.query;
	}

	std::string send_req;

	if (isRobot)
		send_req = "HEAD /robots.txt HTTP/1.0\r\nUser-agent: RituRaj/1.0\r\nHost: " + urlParts.host +
		"\r\nConnection: close\r\n\r\n";
	else
		send_req = "GET " + req + " HTTP/1.0\r\nUser-agent: RituRaj/1.0\r\nHost: " + urlParts.host +
		"\r\nConnection: close\r\n\r\n";

	strcpy_s(req_body, send_req.size() + 1, send_req.c_str());

	bool iResult = socket.socket_send(req_body);
	if (iResult == false)
		return false;

	int read_state = socket.socket_read(isRobot);

	if (read_state == -2 || read_state == -1 || read_state == 0)
		return false;

	incrementBytesRead(socket.get_data_size_inbytes());

	char *status_code;
	char *versionHTTP = strstr(socket.get_webpage_data(), "HTTP/");

	if (versionHTTP == NULL)
		return false;

	versionHTTP = (char *)(versionHTTP + 9); //after 9 position status code will be present
	status_code = (char*)malloc(4);

	std::memcpy(status_code, versionHTTP, 4); //status code length 3 and one for '\0'
	status_code[3] = '\0';

	if (!isRobot)
		incrementHeaderCount(status_code);

	int code = atoi(status_code);

	std::string baseURL = "http://" + urlParts.host;
	char *char_baseURL = new char[INITIAL_BUF_SIZE];
	strcpy_s(char_baseURL, baseURL.size() + 1, baseURL.c_str());
	if (char_baseURL == NULL) { return false; }
	
	if (isRobot && (code < 400 || code >= 500)) {
		return false;
	}
	else if (isRobot) {
		incrementRobotsPassedCount();
		return true;
	}

	if (code == HTTP_STATUS_OK) {
		//incrementCrawledURLCount();

		std::string strHTML(socket.get_webpage_data());
		int headerEndPos = strHTML.find("\r\n\r\n");

		std::string response = strHTML.substr(headerEndPos + 4);
		char *char_response = new char[response.length() + 1];
		strcpy_s(char_response, response.size() + 1, response.c_str());

		int nLinks;
		char *linkBuffer = parser->Parse(char_response, strlen(char_response), char_baseURL, (int)strlen(char_baseURL), &nLinks);

		// check for errors indicated by negative values
		if (nLinks < 0) { nLinks = 0; }
		
		incrementLinksCount(nLinks);
	}
}

void crawlMyPage(UrlParts urlParts, HTMLParserBase *parser) {
	Socket mySocket, robotSocket;
	mySocket.socket_init();
	robotSocket.socket_init();
	struct sockaddr_in server = { 0 };
	struct hostent *remote;
	in_addr addr;
	char *address;

	char *char_host = new char[urlParts.host.size() + 1];
	strcpy_s(char_host, urlParts.host.size() + 1, urlParts.host.c_str());
	DWORD IP = inet_addr(char_host);

	if (IP == INADDR_NONE)
	{
		if ((remote = gethostbyname(char_host)) == NULL)
			return;

		else
		{
			std::memcpy((char *)&(server.sin_addr), remote->h_addr, remote->h_length);
			addr.s_addr = *(u_long *)remote->h_addr;
			address = inet_ntoa(addr);
		}
	}
	else
	{
		server.sin_addr.S_un.S_addr = IP;
		address = char_host;
	}

	incrementDNSCount();
	
	if (checkIPUniqueness(address) == 0)
		return;

	incrementUniqueIPCount();

	server.sin_family = AF_INET;
	server.sin_port = htons(urlParts.port_no);

	if (finishMyCrawl(robotSocket, urlParts, true, server, parser) == 0) // for robots.txt
		return;

	finishMyCrawl(mySocket, urlParts, false, server, parser);
}

void printCurrStatistics(int secCount, int thisPagesCount, float dataThisTime) {
	std::printf("[%3d] %4d Q %6d E %7d H %6d D %6d I %5d R %5d C %5d L %4d\n",
		secCount, stats.getActiveThreadCount(), urlQueue.size(),
		stats.getExtractedURLCount(), stats.getUniqueHostCount(),
		stats.getDNSCount(), stats.getUniqueIPCount(),
		stats.getRobotsPassedCount(), stats.getCrawledURLCount(),
		stats.getLinksCount());
	std::printf("      *** crawling %3d pps @ %.1f Mbps\n", thisPagesCount, dataThisTime);
}

void printFinalStatistics(int secCount) {
	std::cout << endl;
	std::cout << "Final Report" << endl;
	std::cout << "---------------" << endl;
	std::printf("Extracted %7d URLs @ %.1f/s\n", stats.getExtractedURLCount(),
		(float)(stats.getExtractedURLCount() / secCount));
	std::printf("Looked up %7d DNS names @ %.1f/s\n", stats.getDNSCount(),
		((float)stats.getDNSCount() / secCount));
	std::printf("Downloaded %7d robots @ %.1f/s\n", stats.getRobotsPassedCount(),
		((float)stats.getRobotsPassedCount() / secCount));
	std::printf("Crawled %7d pages @ %.1f/s\n", stats.getCrawledURLCount(),
		((float)stats.getCrawledURLCount() / secCount));
	std::printf("Parsed %7d links @ %.1f/s\n", stats.getLinksCount(),
		((float)stats.getLinksCount() / secCount));
	vector<int> headerList = stats.getHeaderCount();
	std::printf("HTTP codes: 2xx = %5d, 3xx = %5d, 4xx = %5d, 5xx = %5d, other = %5d\n",
		headerList.at(0), headerList.at(1), headerList.at(2), headerList.at(3), headerList.at(4));
}