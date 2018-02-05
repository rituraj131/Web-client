#include <iostream>
#include<string>
#include "UrlValidator.h"
#include "GetAndParse.h"
#include "Utility.h";
#include "StatsHandler.h"
#include <fstream>
#include <thread>
using namespace std;

void statsThreadFunc(StatsHandler &);
void crawlerThreadFunc(string, StatsHandler &);

int main(int argc, char **argv)
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
		getAndParse.getAndParseHTML(urlParts, true);
	}
	else if(argc == 3){
		int thread_count = 0;
		Utility utility;

		string threadStr(argv[1]), file_name(argv[2]);
	
		thread_count = utility.getThreadCount(threadStr);

		utility.validateFileExistence(file_name);

		cout << "Opened " << file_name << " with size " << utility.filesize(file_name) << endl;

		string fileContent = utility.readFile(file_name);
		queue<string> urlListQueue = utility.split(fileContent, "\n");

		StatsHandler stats;

		thread statsThread(statsThreadFunc, ref(stats));
		cout << urlListQueue.size() << endl;

		
		thread *crawlerThreads = new thread[thread_count];

		while (urlListQueue.size() != 0) {
			int tCount = 0;
			for (int i = 0; i < thread_count && i < urlListQueue.size(); i++) {
				string url = urlListQueue.front();
				urlListQueue.pop();
				tCount++;
				crawlerThreads[i] = thread(crawlerThreadFunc, url, ref(stats));
			}

			for (int i = 0; i < tCount; i++) {
				crawlerThreads[i].join();
			}
		}

		statsThread.join();

		cout << stats.getBytes() << endl;
	}
	else {
		cout << "Wrong input!" << endl;
	}
	cout << endl;
	return 1; 
}

void statsThreadFunc(StatsHandler &stats) {
	cout << "Started Stats Thread...."<<endl;
}

void crawlerThreadFunc(string url, StatsHandler &stats) {
	cout << "Crawler thread!" << endl;
	stats.incrementBytes(10);
}
