#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>
#include "../io/WriteToFile.h"

#pragma comment(lib, "Ws2_32.lib")
#define DEFAULT_PORT "80"
#define DEFAULT_BUFLEN 5120

namespace sisp {
	class TcpSocket
	{
	private:
		WSADATA wsaData;
		SOCKET ConnectSocket = INVALID_SOCKET;
		struct addrinfo* ptr = NULL,
			hints;

		int initialization();

	public:
		TcpSocket();
		~TcpSocket();

		int urlToIp(const char* address);
		int receive();
	};
}