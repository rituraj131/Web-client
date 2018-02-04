#pragma once

#include <iostream>
using namespace std;
#define INITIAL_BUF_SIZE 8192
#define MAX_BUFF_SIZE_ROBOT 16384
#define MAX_BUFF_SIZE_PAGE 2097152

class Socket
{
private:
	SOCKET sock;
	char *buf;
	int allocatedSize;
	int curPos;
public:
	Socket();
	~Socket();
	bool socket_connect(struct sockaddr_in &server);
	void socket_init();
	int socket_read(bool);
	bool socket_send(char *buf);
	char *get_webpage_data();
	int get_data_size_inbytes();
};

