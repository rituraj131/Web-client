#include "stdafx.h"
#include "MySocket.h"

/*Took reference from https://msdn.microsoft.com/en-us/library/windows/desktop/ms737591(v=vs.85).aspx */

/*
Socket constructor
*/
Socket::Socket()
{
	buf = (char *)malloc(INITIAL_BUF_SIZE);
	allocatedSize = INITIAL_BUF_SIZE;
	curPos = 0;
}

/*
initialize socket
*/
void Socket::socket_init()
{
	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		cout << "WSAStartup failed with error : " << WSAGetLastError() << endl;
		WSACleanup();
		return;
	}

	sock = socket(AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
	{
		cout << "socket failed with error: " << WSAGetLastError() << endl;
		WSACleanup();
		return;
	}
}

/*
connects socket to the server
*/
bool Socket::socket_connect(struct sockaddr_in &server)
{
	if (connect(sock, (struct sockaddr*) &server, sizeof(struct sockaddr_in)) == SOCKET_ERROR)
	{
		return false;
	}
	return true;
}

/*
get HTML content
*/
char *Socket::get_webpage_data()
{
	buf[curPos] = '\0';
	return buf;
}

/*
read HTML content through socket
*/
int Socket::socket_read(bool isRobot)
{
	fd_set sockHolder;
	FD_ZERO(&sockHolder);
	FD_SET(sock, &sockHolder);

	// set timeout to 10 seconds 
	struct timeval timeout;
	timeout.tv_sec = 10;
	timeout.tv_usec = 0;

	DWORD startTime = timeGetTime();

	while (true)
	{
		if (timeGetTime() - startTime >= 10000) {
			return -2;
		}

		int ret = 0;

		if ((ret = select(0, &sockHolder, NULL, NULL, &timeout)) > 0)
		{
			if (buf + curPos == 0)
				break;

			int bytes_recv = recv(sock, (buf + curPos), (allocatedSize - curPos), 0);
			if (bytes_recv < 0)
			{
				break;
			}
			if (bytes_recv == 0)
			{
				//cout << buf << endl;
				return 1;           // normal completion 
			}
			curPos += bytes_recv;               // adjust where the next recv goes 
			if (allocatedSize - curPos < 512)
			{
				allocatedSize = allocatedSize << 1;
				char *nullCheck = (char *)realloc(buf, allocatedSize);
				if (nullCheck != NULL)
				{
					buf = nullCheck;
				}
				else
				{
					free(buf);
					break;
				}
			}

			if (isRobot == 1 && curPos > MAX_BUFF_SIZE_ROBOT)
				return 0;

			if (isRobot == 0 && curPos > MAX_BUFF_SIZE_PAGE)
				return 0;
		}
		else
		{
			break;
		}
	}
	return -1;
}

/*
get number of bytes read
*/
int Socket::get_data_size_inbytes()
{
	return curPos > 0 ? curPos - 1 : curPos;
}

/*
sends request string
*/
bool Socket::socket_send(char *buf)
{
	int ret = 0;
	ret = send(sock, buf, strlen(buf), 0);
	if (ret == SOCKET_ERROR) {
		printf("send failed: %d\n", WSAGetLastError());
		WSACleanup();
		return false;
	}
	return true;
}

Socket::~Socket()
{
	//free(buf);
	closesocket(this->sock);
}
