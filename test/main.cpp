#include "common.h"
#include "UrlValidator.h"
#include "GetAndParse.h"
#include "Utility.h";
#include "Stats.h"
#include "ThreadPool.h"

bool done = false;

int main(int argc, char **argv) {
	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		cout << "WSAStartup failed with error : " << WSAGetLastError() << endl;
		WSACleanup();
		return 0;
	}

	if (argc == 2) {
		std::string url(argv[1]);
		cout << "URL: " << url << endl;
		UrlValidator validate;
		UrlParts urlParts = validate.urlParser(url, true);

		if (urlParts.isValid == -10)//some failure in parsing observed!
			return 0;

		GetAndParse getAndParse;
		getAndParse.getAndParseHTML(urlParts, true);
	}
	else if (argc == 3) {
		string threadStr(argv[1]), file_name(argv[2]);
		Utility utility;
		int thread_count = utility.getThreadCount(threadStr);

		if (!utility.validateFileExistence(file_name)) {
			cout << "\n Invalid File Name\n";
			exit(0);
		}

		cout << "Opened..\\" << file_name << " with size " << utility.filesize(file_name) << endl;
		ThreadPool tPool = ThreadPool();
		tPool.letTheGameBegin(thread_count, file_name);
	}
	else {
		cout << "Wrong input!" << endl;
	}
	cout << endl;
	WSACleanup();

	return 0;
}