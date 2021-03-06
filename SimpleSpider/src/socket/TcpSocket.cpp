#include "TcpSocket.h"

using namespace sisp;

TcpSocket::TcpSocket() {
	initialization();
}

TcpSocket::~TcpSocket() {}

int TcpSocket::initialization() {
	int returnCode;

	// Initialize Winsock
	returnCode = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (returnCode != 0) {
		printf("WSAStartup failed: %d\n", returnCode);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	return 0;
}

int TcpSocket::urlToIp(const char* address)
{
	int returnCode;

	// Resolve the server address and port
	returnCode = getaddrinfo(address, DEFAULT_PORT, &hints, &ptr);
	if (returnCode != 0) {
		printf("getaddrinfo failed: %d\n", returnCode);
		WSACleanup();
		return 1;
	}

	return 0;
}

int TcpSocket::receive()
{
	// Create a SOCKET for connecting to server
	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
		ptr->ai_protocol);

	// Check if socket is valid or not
	if (ConnectSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(ptr);
		WSACleanup();
		return 1;
	}

	const char* sendbuf = "GET / HTTP/1.0\r\n\r\n";

	int connectionCode;

	connectionCode = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (connectionCode == SOCKET_ERROR) {
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}

	connectionCode = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
	if (connectionCode == SOCKET_ERROR) {
		printf("Send Failed %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	printf("Bytes Sent: %ld\n", connectionCode);

	connectionCode = shutdown(ConnectSocket, SD_SEND);
	if (connectionCode == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	FileIO myFile("example.html"); //TODO: Dynamic naming
	char recvbuf[DEFAULT_BUFLEN];
	char *foundPtr;
	bool runFinder = true;
	int difference = 0;
	do {
		connectionCode = recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN - 1, 0);
		if (connectionCode > 0) {
			//printf("Bytes recieved: %d\n", connectionCode);
			if (runFinder) {
				foundPtr = Parser::removeHeader(recvbuf);
				recvbuf[connectionCode] = '\0';
				myFile.writeToFile(foundPtr);
				runFinder = false;
			}
			else {
				recvbuf[connectionCode] = '\0';
				myFile.writeToFile(recvbuf);
			}
		}
		else if (connectionCode == 0)
			printf("Connection close\n");
		else
			printf("recv failed: %d\n", WSAGetLastError());
	} while (connectionCode > 0);

	//call function to parse out header
	

	// cleanup
	myFile.closeFile();
	closesocket(ConnectSocket);
	WSACleanup();

	return 0;
}
	