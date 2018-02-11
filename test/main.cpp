#include "common.h"
#include "UrlValidator.h"
#include "GetAndParse.h"
#include "Utility.h";
#include "Stats.h"
#include "ThreadPool.h"

bool done = false;

int main(int argc, char **argv) {
	if (argc == 2) {
		std::string url;
		cout << "Input URL: ";
		cin >> url;

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
		
		ThreadPool tPool = ThreadPool();
		tPool.letTheGameBegin(thread_count, file_name);
	}
	else {
		cout << "Wrong input!" << endl;
	}
	cout << endl;
}