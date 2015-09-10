#ifndef SERVER_NETWORK_H
#define SERVER_NETWORK_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <IPHlpApi.h>
#include <process.h>
#include <stdio.h>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "12131"
#define DEFAULT_BUFLEN 512

class ServerNetwork
{
private:
	SOCKET _listen_socket;
	std::vector<HANDLE> threadVector;

	static unsigned int _stdcall receiving_data(void *socket);
public:
	ServerNetwork();
	~ServerNetwork();

	void listening(unsigned int (_stdcall *callback)(void*));

	bool send_to_client(SOCKET socket, char *packets, int length);
};

#endif //SERVER_NETWORK_H