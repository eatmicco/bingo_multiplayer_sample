#ifndef CLIENT_NETWORK_H
#define CLIENT_NETWORK_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif //WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <IPHlpApi.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_IP "localhost"
#define DEFAULT_PORT "12131"
#define DEFAULT_BUFLEN 512

class ClientNetwork
{
private:
	SOCKET _server_socket;
	struct addrinfo *_server_addr;

public:
	ClientNetwork();
	~ClientNetwork();

	bool connect_to_server();

	void send_to_server(char *packets, int total_size);

	int receive_packets(char *);
};

#endif //CLIENT_NETWORK_H