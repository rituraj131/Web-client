#pragma once
#include "common.h"

class PrevHost
{
private:
	std::unordered_set<DWORD> seenIPs;	std::unordered_set<std::string> seenHosts;
public:
	bool checkIfHostUnique(std::string);
	bool checkIfIPUnique(std::string ip);
};

