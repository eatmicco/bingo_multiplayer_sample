#include "bingo_board.h"
#include "bingo_data.h"
#include "server_network.h"
#include <stdio.h>
#include <vector>

BingoBoard board(3);
ServerNetwork network;
std::vector<SOCKET> _client_sockets;

void broadcast_to_clients(char *buffer, int length)
{
	auto itr = _client_sockets.begin();
	for (; itr != _client_sockets.end(); itr++)
	{
		if (!network.send_to_client(*itr, buffer, length))
			printf("Error sending to client.\n");
	}
}

unsigned int _stdcall receiving_data(void *data)
{
	char buffer[512];
	int result, send_result;
	int buflen = 512;
	SOCKET client_socket = (SOCKET)data;

	_client_sockets.push_back(client_socket);

	//receive until the peer shuts down the connection
	do {
		result = recv(client_socket, buffer, buflen, 0);
		if (result > 0) {
			BingoData *data = (BingoData*)buffer;
			if (data->type == BingoDataType::PLAY) {
				int id = board.add_new_player(data->param.representation.icon);

				BingoData send_data;
				strcpy(send_data.key, "DATA");
				send_data.type = BingoDataType::PLAY;
				send_data.player_id = id;
				if (network.send_to_client(client_socket, (char*)&send_data, sizeof(send_data)))
					printf("Player %d entered.\n", id);

				if (board.get_player_count() > 1)
				{
					BingoData turn_data;
					strcpy(turn_data.key, "DATA");
					turn_data.type = BingoDataType::TURN;
					turn_data.player_id = 1;
					broadcast_to_clients((char*)&turn_data, sizeof(turn_data));
				}
			}
			else if (data->type == BingoDataType::PUT) {
				printf("Put Item to Board\n");
				board.put_item_to_board(data->param.pos.x, data->param.pos.y, data->player_id);
				printf("Checking Bingo...\n");
				int id;
				if (board.check_bingo(id))
				{
					BingoData send_data;
					strcpy(send_data.key, "DATA");
					send_data.type = BingoDataType::RESULT;
					send_data.player_id = data->player_id;
					send_data.param.result = 1;
					broadcast_to_clients((char*)&send_data, sizeof(send_data));
					printf("Bingo!\n");
				}
				else
				{
					BingoData send_data;
					strcpy(send_data.key, "DATA");
					send_data.type = BingoDataType::RESULT;
					send_data.player_id = -1;
					send_data.param.result = 0;
					broadcast_to_clients((char*)&send_data, sizeof(send_data));
					printf("Not yet Bingo.\n");

					char view[BOARD_SIDE * BOARD_SIDE];
					board.create_view(view);
					broadcast_to_clients(view, sizeof(view));

					BingoData turn_data;
					strcpy(turn_data.key, "DATA");
					turn_data.type = BingoDataType::TURN;
					int next_id = (data->player_id % board.get_player_count()) + 1;
					turn_data.player_id = next_id;
					broadcast_to_clients((char*)&turn_data, sizeof(turn_data));
				}
			}
			else if (data->type == BingoDataType::SHOW)
			{
				char view[BOARD_SIDE * BOARD_SIDE];
				board.create_view(view);
				broadcast_to_clients(view, sizeof(view));
			}

		} else if (result == 0) {
			printf("Connection closing...\n");
		} else {
			printf("recv failed: %d\n", WSAGetLastError());
			closesocket(client_socket);
			WSACleanup();
			return 1;
		}
	} while (result > 0);

	//shut down the send half of the connection since no more data will be sent
	result = shutdown(client_socket, SD_SEND);
	if (result == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(client_socket);
		WSACleanup();
		return 1;
	}

	return 0;
}

int main(int argc, char **argv)
{
	printf("====Bingo Server====\n");

	network.listening(&receiving_data);
}
