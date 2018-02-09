#include "GetAndParse.h"

static PrevHost prevHost;
struct sockaddr_in server = { 0 };

/*
parse and return number of links in the webpage,
webpage content is in fileBuf, baseUrl is the host part of webpage url
*/
int getLinkCount(char *fileBuf, char *baseUrl)
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

/*
utility function to print data for debugging
*/
void printData(UrlParts urlParts) {
	cout << endl << "print:: Parsed URL" << endl;
	cout << "--------------" << endl;
	cout << "scheme: " << urlParts.scheme << endl;
	cout << "host: " << urlParts.host << endl;
	cout << "port: " << urlParts.port_no << endl;
	cout << "path: " << urlParts.path << endl;
	cout << "query: " << urlParts.query << endl;
}

bool handleHostUniquenessCheck(UrlParts urlParts) {
	cout << "\t  Checking host uniqueness... ";
	if (prevHost.checkIfHostUnique(urlParts.host) == 0) {
		cout << "failed" << endl;
		return false;
	}
	else {
		cout << "passed" << endl;
		return true;
	}
}

bool handleIPUniquenessCheck(char *address) {
	cout << "\t  Checking IP uniqueness... ";
	if (prevHost.checkIfIPUnique(address) == 0) {
		cout << "failed" << endl;
		return false;
	}
	else {
		cout << "passed" << endl;
	}

	return true;
}


void printReadData(std::string data) {
	int pos = 0;
	cout << "printData\n";
	while (data.length() > pos) {
		int newPos = min(data.length(), pos + 200);
		cout <<data.substr(pos, newPos) << endl;
		pos = newPos;
		Sleep(20000);
	}
}

bool justdoit(Socket socket, UrlParts urlParts, bool isRobot, bool isPrintHeader) {
	DWORD startTime = timeGetTime();
	if (isRobot == 1)
		cout << "\t  Connecting on robots... ";
	else
		cout << "\t* Connecting on page... ";

	if (!socket.socket_connect(server))
	{
		cout << "failed with " << WSAGetLastError() << endl;
		return false;
	}

	cout << "done in " << timeGetTime() - startTime << " ms" << endl;

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
		send_req = "HEAD /robots.txt HTTP/1.1\r\nUser-agent: RituRaj/1.0\r\nHost: " + urlParts.host +
		"\r\nConnection: close\r\n\r\n";
	else
		send_req = "GET " + req + " HTTP/1.1\r\nUser-agent: RituRaj/1.0\r\nHost: " + urlParts.host +
		"\r\nConnection: close\r\n\r\n";

	strcpy_s(req_body, send_req.size() + 1, send_req.c_str());

	bool iResult = socket.socket_send(req_body);
	if (iResult == false)
	{
		cout << "\n\t  failed with sending GET " << WSAGetLastError() << endl;
		return false;
	}

	startTime = timeGetTime();
	cout << "\t  Loading... ";
	int read_state = socket.socket_read(isRobot);
	if (read_state == -2) {
		cout << "failed with slow download" << endl;
		return false;
	}
	else if (read_state == -1)
	{
		cout << "failed with read " << WSAGetLastError() << endl;
		return false;
	}
	else if (read_state == 0) {
		cout << "failed with exceeding max" << endl;
		return false;
	}

	cout << "done in " << timeGetTime() - startTime << " ms with " << socket.get_data_size_inbytes() << " bytes" << endl;
	//if (!isRobot)printReadData(socket.get_webpage_data());
	cout << "\t  Verifying header... ";
	char *status_code;
	char *versionHTTP = strstr(socket.get_webpage_data(), "HTTP/");

	if (versionHTTP == NULL)
	{
		cout << "failed with non-HTTP header" << endl;
		return false;
	}

	versionHTTP = (char *)(versionHTTP + 9); //after 9 position status code will be present
	status_code = (char*)malloc(4);

	memcpy(status_code, versionHTTP, 4); //status code length 3 and one for '\0'
	status_code[3] = '\0';

	int code = atoi(status_code);

	cout << " status code " << code << endl;
	free(status_code);

	std::string baseURL = "http://" + urlParts.host;
	char *char_baseURL = new char[INITIAL_BUF_SIZE];
	strcpy_s(char_baseURL, baseURL.size() + 1, baseURL.c_str());
	if (char_baseURL == NULL) { return false; }

	std::string strHTML(socket.get_webpage_data());
	int headerEndPos = strHTML.find("\r\n\r\n");

	if (isRobot && code < 400 || code >= 500) {
		return false;
	}

	if (isRobot == 0 && code == HTTP_STATUS_OK)
	{
		startTime = timeGetTime();
		cout << "\t+ Parsing page... ";
		std::string response = strHTML.substr(headerEndPos + 4);
		char *char_response = new char[response.length() + 1];
		strcpy_s(char_response, response.size() + 1, response.c_str());
		int nLinks = getLinkCount(char_response, char_baseURL);

		cout << "done in " << timeGetTime() - startTime << " ms with " << nLinks << " links" << endl;
	}

	//free(char_baseURL);

	if (isPrintHeader) {
		std::string strHeader = strHTML.substr(0, headerEndPos);
		cout << "\n\n--------------------------------------------------" << endl;
		cout << strHeader << endl << endl << endl;
	}

	return true;
}

void GetAndParse::getAndParseHTML(UrlParts urlParts, bool isPrintHeader) {
	if (handleHostUniquenessCheck(urlParts) == 0)
		return;

	//initilize socket
	Socket mySocket, robotSocket;
	mySocket.socket_init();
	robotSocket.socket_init();

	struct hostent *remote;

	//struct sockaddr_in server;
	in_addr addr;
	char *address;

	char *char_host = new char[urlParts.host.size() + 1];
	strcpy_s(char_host, urlParts.host.size() + 1, urlParts.host.c_str());
	DWORD IP = inet_addr(char_host);

	DWORD startTime = timeGetTime();
	cout << "\t  Doing DNS... ";

	if (IP == INADDR_NONE)
	{
		// if not a valid IP, then do a DNS lookup
		if ((remote = gethostbyname(char_host)) == NULL)
		{
			cout << "failed with " << WSAGetLastError() << endl;
			return;
		}
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

	cout << "done in " << timeGetTime() - startTime << " ms, found " << address << endl;

	if (handleIPUniquenessCheck(address) == 0)
		return;

	// setup the port # and protocol type
	server.sin_family = AF_INET;
	server.sin_port = htons(urlParts.port_no);

	//if (justdoit(robotSocket, urlParts, true, false) == 0)
		//return;
	if (justdoit(mySocket, urlParts, false, isPrintHeader) == 0)
		return;
}