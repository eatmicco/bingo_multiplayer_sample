#include "server_network.h"

ServerNetwork::ServerNetwork()
{
	WSADATA wsaData;

	int _result;

	//Initialize Winsock
	_result = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (_result != 0)
	{
		printf("WSAStartup failed: %d\n", _result);
		return;
	}

	struct addrinfo *result = NULL, *ptr = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	//Resolve the local address and port to be used by the server
	_result = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (_result != 0) {
		printf("getaddrinfo failed: %d\n", _result);
		WSACleanup();
		return;
	}

	//create socket
	_listen_socket = INVALID_SOCKET;
	_listen_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (_listen_socket == INVALID_SOCKET)
	{
		printf("Error at socket(): %d\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return;
	}

	//setup the TCP listening socket
	_result = bind(_listen_socket, result->ai_addr, (int)result->ai_addrlen);
	if (_result == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(_listen_socket);
		WSACleanup();
		return;
	}

	//no longer needed
	freeaddrinfo(result);

	if (listen(_listen_socket, SOMAXCONN) == SOCKET_ERROR)
	{
		printf("Listen failed with error: %d\n", WSAGetLastError());
		closesocket(_listen_socket);
		WSACleanup();
		return;
	}
}

ServerNetwork::~ServerNetwork()
{
	//cleanup
	closesocket(_listen_socket);
	WSACleanup();
}

void ServerNetwork::listening(unsigned int (_stdcall *callback)(void*))
{
	//accepting a connection
	SOCKET client_socket;

	//HACK : for now, don't have any means to close except for X button.
	while (true)
	{
		client_socket = INVALID_SOCKET;
		//accept a client socket
		client_socket = accept(_listen_socket, NULL, NULL);
		if (client_socket == INVALID_SOCKET) {
			printf("Accept failed: %d\n", WSAGetLastError());			
		}
		printf("Client accepted\n");

		HANDLE myThread = (HANDLE)_beginthreadex(0, 0, callback, (void*)client_socket, 0, 0);
		threadVector.push_back(myThread);
	}

	/*
	WaitForMultipleObjects(threadVector.size(), &threadVector[0], true, INFINITE);

	//cleanup
	closesocket(client_socket);
	closesocket(_listen_socket);
	WSACleanup();
	*/
}

bool ServerNetwork::send_to_client(SOCKET socket, char *packets, int length)
{
	int result = send(socket, packets, length, 0);
	if (result == SOCKET_ERROR) {
		printf("send failed: %d\n", WSAGetLastError());
		closesocket(socket);
		WSACleanup();
		return false;
	}

	return true;
}