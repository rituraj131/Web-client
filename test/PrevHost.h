#pragma once
#include <iostream>
#include <unordered_set>
#include<string>
#include <windows.h>
using namespace std;

class PrevHost
{
private:
	std::unordered_set<DWORD> seenIPs;	std::unordered_set<std::string> seenHosts;
public:
	bool checkIfHostUnique(std::string);
	bool checkIfIPUnique(std::string ip);
};

