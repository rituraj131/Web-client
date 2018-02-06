#include "Crawler.h"

static struct sockaddr_in server = { 0 };
static mutex mtx;
static PrevHost prevHost;

void incrementDNSCount(Stats &stats) {
	mtx.lock();
	stats.incrementDNSCount();
	mtx.unlock();
}

void incrementUniqueIPCount(Stats &stats) {
	mtx.lock();
	stats.incrementUniqueIPCount();
	mtx.unlock();
}

void incrementBytesRead(Stats &stats, long bytesRead) {
	mtx.lock();
	stats.incrementBytesRead(bytesRead);
	mtx.unlock();
}

void incrementRobotsPassedCount(Stats &stats) {
	mtx.lock();
	stats.incrementRobotsPassedCount();
	mtx.unlock();
}

void incrementCrawledURLCount(Stats &stats) {
	mtx.lock();
	stats.incrementCrawledURLCount();
	mtx.unlock();
}

void incrementLinksCount(Stats &stats, int count) {
	mtx.lock();
	stats.incrementLinksCount(count);
	mtx.unlock();
}

bool checkIPUniquenessCrawler(char *address) {
	mtx.lock();
	bool ret = true;
	if (prevHost.checkIfIPUnique(address) == 0)
		ret = false;
	mtx.unlock();
	return ret;
}

int getLinkCountCrawler(char *fileBuf, char *baseUrl)
{
	// create new parser object
	HTMLParserBase *parser = new HTMLParserBase;

	int nLinks;
	char *linkBuffer = parser->Parse(fileBuf, strlen(fileBuf), baseUrl, (int)strlen(baseUrl), &nLinks);

	// check for errors indicated by negative values
	if (nLinks < 0) { nLinks = 0; }

	delete parser;		// this internally deletes linkBuffer
	return nLinks;
}

bool crawlRealDeal(Stats &stats, Socket socket, UrlParts urlParts, bool isRobot) {
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
	if (read_state == -2)//slow download
		return false;
	
	else if (read_state == -1)
	{
		return false;
	}
	else if (read_state == 0)//exceeding max read limit
		return false;

	incrementBytesRead(ref(stats), socket.get_data_size_inbytes());

	char *status_code;
	char *versionHTTP = strstr(socket.get_webpage_data(), "HTTP/");

	if (versionHTTP == NULL)
		return false;

	versionHTTP = (char *)(versionHTTP + 9); //after 9 position status code will be present
	status_code = (char*)malloc(4);

	memcpy(status_code, versionHTTP, 4); //status code length 3 and one for '\0'
	status_code[3] = '\0';

	int code = atoi(status_code);

	std::string baseURL = "http://" + urlParts.host;
	char *char_baseURL = new char[INITIAL_BUF_SIZE];
	strcpy_s(char_baseURL, baseURL.size() + 1, baseURL.c_str());
	if (char_baseURL == NULL) { return false; }
	//cout << "code: " << code << " isRobot: "<<isRobot<<endl;
	if (isRobot && code < 400 || code >= 500) {
		return false;
	}
	else if (isRobot) {
		incrementRobotsPassedCount(ref(stats));
		return true;
	}
	//cout << "code: " << code << endl;
	if (code == HTTP_STATUS_OK) {
		incrementCrawledURLCount(ref(stats));

		std::string strHTML(socket.get_webpage_data());
		int headerEndPos = strHTML.find("\r\n\r\n");

		std::string response = strHTML.substr(headerEndPos + 4);
		char *char_response = new char[response.length() + 1];
		strcpy_s(char_response, response.size() + 1, response.c_str());
		int nLinks = getLinkCountCrawler(char_response, char_baseURL);
		incrementLinksCount(ref(stats), nLinks);
	}
}

void Crawler::crawl(Stats &stats, UrlParts urlParts) {
	Socket mySocket, robotSocket;
	mySocket.socket_init();
	robotSocket.socket_init();

	struct hostent *remote;
	in_addr addr;
	char *address;

	char *char_host = new char[urlParts.host.size() + 1];
	strcpy_s(char_host, urlParts.host.size() + 1, urlParts.host.c_str());
	DWORD IP = inet_addr(char_host);

	if (IP == INADDR_NONE)
	{
		// if not a valid IP, then do a DNS lookup
		if ((remote = gethostbyname(char_host)) == NULL)
			return;
		
		else // take the first IP address and copy into sin_addr
		{
			memcpy((char *)&(server.sin_addr), remote->h_addr, remote->h_length);
			addr.s_addr = *(u_long *)remote->h_addr;
			address = inet_ntoa(addr);
		}
	}
	else
	{
		// if a valid IP, directly drop its binary version into sin_addr
		server.sin_addr.S_un.S_addr = IP;
		address = char_host;
	}

	incrementDNSCount(ref(stats));
	if (checkIPUniquenessCrawler(address) == 0)
		return;

	incrementUniqueIPCount(ref(stats));

	server.sin_family = AF_INET;
	server.sin_port = htons(urlParts.port_no);

	if (crawlRealDeal(ref(stats), robotSocket, urlParts, true) == 0)
		return;

	crawlRealDeal(ref(stats), mySocket, urlParts, false);
}
