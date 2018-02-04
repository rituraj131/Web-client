#include "PrevHost.h"

bool PrevHost::checkIfHostUnique(std::string host) {
	unordered_set<string>::const_iterator found = seenHosts.find(host);

	if (found == seenHosts.end()) {//host was not seen before
		seenHosts.insert(host);
		return true;
	}
	else {
		return false;
	}
}
	
bool PrevHost::checkIfIPUnique(std::string ip) {
	char *char_ip = new char[ip.size() + 1];
	strcpy_s(char_ip, ip.size() + 1, ip.c_str());
	DWORD DWORD_IP = inet_addr(char_ip);

	unordered_set<DWORD>::const_iterator found = seenIPs.find(DWORD_IP);

	if (found == seenIPs.end()) {//ip was not seen before
		seenIPs.insert(DWORD_IP);
		return true;
	}
	else {
		return false;
	}
}
