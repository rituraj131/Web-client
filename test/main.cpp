#include <iostream>
#include <vector>
#include<string>
#include "UrlValidator.h"
#include "GetAndParse.h"
#include "Utility.h";
#include <fstream>
using namespace std;

int main(int argc, char **argv)
{
	cout << "hello World!" << argc << endl;
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
		std::vector<std::string> urlList = utility.split(fileContent, "\n");

		GetAndParse getAndParse;
		UrlValidator validate;
		int urlCount = 0;

		std::ifstream ifs(file_name);
		if (ifs) {
			string url;
			
			while (std::getline(ifs, url)) {
				if (url.length() == 0) continue;

				cout << "URL: " << url << endl;
				UrlParts urlParts = validate.urlParser(url);
				if (urlParts.isValid == -10)//some failure in parsing observed!
					continue;

				getAndParse.getAndParseHTML(urlParts, false);
				urlCount++;
			}
		}
		else {
			cout << "Unable to read file, Exiting!" << endl;
			exit(0);
		}

		if (urlCount == 0) {
			cout<<"No URLs found"<<endl;
		}
	}
	else {
		cout << "Wrong input!" << endl;
	}
	cout << endl;
	return 1; 
}
