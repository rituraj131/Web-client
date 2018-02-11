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
		return false;
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
queue<string> Utility::split(std::string s, string delimiter) {
	queue<string> q;

	size_t pos = 0;
	std::string token;
	while ((pos = s.find(delimiter)) != std::string::npos) {
		token = s.substr(0, pos);
		q.push(token);
		s.erase(0, pos + delimiter.length());
	}

	q.push(s);

	return q;
}

unsigned long Utility::HexToDec(char hex[]) {
	char *hexstr;
	int length = 0;
	const int base = 16;     // Base of Hexadecimal Number
	unsigned long decnum = 0;
	int i;

	// Now Find the length of Hexadecimal Number
	for (hexstr = hex; *hexstr != '\0'; hexstr++) {
		length++;
	}

	// Now Find Hexadecimal Number
	hexstr = hex;
	for (i = 0; *hexstr != '\0' && i < length; i++, hexstr++) {
		// Compare *hexstr with ASCII values
		if (*hexstr >= 48 && *hexstr <= 57) {  // is *hexstr Between 0-9
			decnum += (((int)(*hexstr)) - 48) * pow(base, length - i - 1);
		}
		else if ((*hexstr >= 65 && *hexstr <= 70)) {  // is *hexstr Between A-F
			decnum += (((int)(*hexstr)) - 55) * pow(base, length - i - 1);
		}
		else if (*hexstr >= 97 && *hexstr <= 102) {  // is *hexstr Between a-f
			decnum += (((int)(*hexstr)) - 87) * pow(base, length - i - 1);
		}
		else {
			std::cout << "Invalid Hexadecimal Number \n";

		}
	}
	return decnum;
}

bool Utility::isItChunked(string header) {
	string line;
	const string chunked_ident_str = "Transfer-Encoding:  chunked";

	int pos = 0;
	string buff = "";
	while (pos < header.length()) {
		pos = header.find_first_of("\r\n");
		line = header.substr(0, pos);

		if (line.compare(chunked_ident_str) == 0)
			return true;

		pos += 2;
		header = header.substr(pos);
	}
	return false;
}

void Utility::printURLList(vector<string> list) {
	for (int i = 0; i < list.size(); i++)
		cout << list.at(i) << endl;
}

void Utility::printData(UrlParts urlParts) {
	cout << endl << "print:: Parsed URL" << endl;
	cout << "--------------" << endl;
	cout << "scheme: " << urlParts.scheme << endl;
	cout << "host: " << urlParts.host << endl;
	cout << "port: " << urlParts.port_no << endl;
	cout << "path: " << urlParts.path << endl;
	cout << "query: " << urlParts.query << endl;
}

