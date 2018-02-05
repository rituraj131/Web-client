#include "Utility.h"

//Reference from https://stackoverflow.com/a/5840160/4135902
std::ifstream::pos_type Utility::filesize(std::string filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg(); 
}

int Utility::getThreadCount(std::string count) {
	try {
		int thread_count = stoi(count);
		return thread_count;
	}
	catch (std::invalid_argument& e) {
		cout << "\nPlease try again with Integer value for thread count! Exiting..\n";
		exit(0);
	}
}

int Utility::validateThreadCount(std::string count) {
	try {
		int thread_count = stoi(count);
		if (thread_count != 1) {
			cout << "\n Please try again with Thread Count 1, Exiting...\n";
			exit(0);
		}
		return thread_count;
	}
	catch (std::invalid_argument& e) {
		cout << "\nPlease try again with Integer value for thread count! Exiting..\n";
		exit(0);
	}
}

//reference from https://stackoverflow.com/a/33486052/4135902
bool Utility::validateFileExistence(const std::string &Filename)
{
	if (access(Filename.c_str(), 0) == 0)
		return true;
	else {
		cout << "File Not Found! Exiting..." << endl;
		exit(0);
	}
}

//reference from https://stackoverflow.com/a/2912614/4135902
std::string Utility::readFile(std::string file_name) {
	std::ifstream ifs(file_name);
	if (ifs) {
		std::string content((std::istreambuf_iterator<char>(ifs)),
			(std::istreambuf_iterator<char>()));
		return content;
	}
	else {
		cout << "Unable to read file, Exiting!" << endl;
		exit(0);
	}
}

//reference from https://stackoverflow.com/a/14266139/4135902
vector<string> Utility::split(std::string s, string delimiter) {
	vector<string> list;

	size_t pos = 0;
	std::string token;
	while ((pos = s.find(delimiter)) != std::string::npos) {
		token = s.substr(0, pos);
		list.push_back(token);
		s.erase(0, pos + delimiter.length());
	}

	list.push_back(s);

	return list;
}

void Utility::printURLList(vector<string> list) {
	for (int i = 0; i < list.size(); i++)
		cout << list.at(i) << endl;
}

vector<UrlParts> Utility::getURLList(vector<string> list) {
	UrlValidator validate;
	vector<UrlParts> urlPartsList;
	for (int i = 0; i < list.size(); i++)
		urlPartsList.push_back(validate.urlParser(list.at(i)));

	return urlPartsList;
}

