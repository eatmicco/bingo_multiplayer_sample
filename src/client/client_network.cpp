#include "client_network.h"

ClientNetwork::ClientNetwork()
{
	WSADATA wsaData;

	int _result;

	//Initialize Winsock
	_result = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (_result != 0) {
		printf("WSAStartup failed: %d\n", _result);
		return;
	}

	struct addrinfo *result = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	//Resolve the server address and port
	_result = getaddrinfo(DEFAULT_IP, DEFAULT_PORT, &hints, &result);
	if (_result != 0)
	{
		printf("getaddrinfo failed: %d\n", _result);
		WSACleanup();
		return;
	}

	_server_socket = INVALID_SOCKET;

	//Attempt to connect to the first address returned by 
	//the call to getaddrinfo
	_server_addr = result;

	//Create a SOCKET for connecting to server
	_server_socket = socket(_server_addr->ai_family, _server_addr->ai_socktype, _server_addr->ai_protocol);
	if (_server_socket == INVALID_SOCKET)
	{
		printf("Error at socket(): %d\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return;
	}
}

ClientNetwork::~ClientNetwork()
{
	//cleanup
	closesocket(_server_socket);
	WSACleanup();
}

bool ClientNetwork::connect_to_server()
{
	int _result;
	//connect to server
	_result = connect(_server_socket, _server_addr->ai_addr, (int)_server_addr->ai_addrlen);
	if (_result == SOCKET_ERROR) {
		closesocket(_server_socket);
		_server_socket = INVALID_SOCKET;
	}

	//should really try the next address returned by getaddrinfo
	//if the connect call failed
	//but for this simple example we just free the resources
	//returned by getaddrinfo and print an error message

	freeaddrinfo(_server_addr);

	if (_server_socket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return false;
	}

	return true;
}

void ClientNetwork::send_to_server(char *packets, int total_size)
{
	int result;
	//Send an initial buffer
	result = send(_server_socket, packets, total_size, 0);
	if (result == SOCKET_ERROR) {
		printf("send failed: %d\n", WSAGetLastError());
		closesocket(_server_socket);
		WSACleanup();
	}
}

int ClientNetwork::receive_packets(char *packets)
{
	int result;
	result = recv(_server_socket, packets, DEFAULT_BUFLEN, 0);
	if (result == 0)
	{
		printf("Connection closed\n");
	}
	else if (result < 0)
	{
		printf("recv failed: %d\n", WSAGetLastError());
	}

	return result;
}