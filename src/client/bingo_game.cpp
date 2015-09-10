#include "bingo_game.h"
#include "bingo_data.h"
#include <process.h>
#include <stdio.h>
#include <stdlib.h>

BingoGame::BingoGame() : _finish(false), _waiting_data(false), _player_id(-1), _in_turn(true)
{
}

void BingoGame::run()
{
	printf("Welcome to Bingo!\n");
	printf("Connect to server? ");
	char command[256];
	get_command(command, 256);
	
	size_t l = strlen(command);
	if (l > 0 && strcmp(command, "yes") == 0)
	{
		printf("Connecting to server...\n");
		_network = new ClientNetwork();
		if(_network->connect_to_server())
		{
			printf("Connected to server.\n");

			//create new bg thread for receiving data
			_bg_thread = (HANDLE)_beginthreadex(0, 0, &(BingoGame::receiving_data), (void*)this, 0, 0);

			while (!_finish)
			{
				if (!_waiting_data && _in_turn)
					query_command();
			}
		}
	}
}

void BingoGame::get_command(char *buffer, int len)
{
	if (fgets(buffer, len, stdin) != 0)
	{
		size_t l = strlen(buffer);
		if (l > 0 && buffer[l-1] == '\n')
			buffer[l-1] = '\0';
	}
}

void BingoGame::query_command()
{
	char command[256];
	printf("Enter Command : ");
	get_command(command, 256);

	size_t l = strlen(command);
	if (l > 0 && strcmp(command, "play") == 0)
	{
		char icon[3];
		printf("icon : ");
		get_command(icon, 3);
		if (_player_id == -1) {
			//_player_id = board.add_new_player();
			BingoData data;
			strcpy(data.key, "DATA");
			data.type = BingoDataType::PLAY;
			data.param.representation.icon = icon[0];
			_network->send_to_server((char*)&data, sizeof(data));
			_waiting_data = true;
			_in_turn = false;
		}
		else
			printf("Already in the game.");
	}
	else if (l > 0 && strcmp(command, "put") == 0)
	{
		char pos[4];
		int x = -1, y = -1;
		printf("x : ");
		get_command(pos, 4);
		l = strlen(pos);
		if (l > 0 && l < 3)
		{
			x = atoi(pos);
		}
		printf("y : ");
		get_command(pos, 4);
		l = strlen(pos);
		if (l > 0 && l < 3)
		{
			y = atoi(pos);
		}
		if (x > -1 && y > -1)
		{
			//board.put_item_to_board(x, y, _player_id);
			BingoData data;
			strcpy(data.key, "DATA");
			data.type = BingoDataType::PUT;
			data.player_id = _player_id;
			data.param.pos.x = x;
			data.param.pos.y = y;
			_network->send_to_server((char*)&data, sizeof(data));
			_waiting_data = true;
			_in_turn = false;
		}
	}
	else if (l > 0 && strcmp(command, "show") == 0)
	{
		//board.show();
		BingoData data;
		strcpy(data.key, "DATA");
		data.type = BingoDataType::SHOW;
		_network->send_to_server((char*)&data, sizeof(data));
		_waiting_data = true;
	}
	printf("\n");
}

unsigned int BingoGame::receiving_data(void *param)
{
	int result;
	char buffer[256];
	BingoGame *game = (BingoGame*)param;
	if (!game) return 0;

	while (true)
	{
		result = game->_network->receive_packets(buffer);
		if (result == 0)	//close connection
		{
			break;
		}
		char key[5];
		strncpy(key, buffer, 5);
		key[4] = '\0';
		if (strcmp(key, "DATA") == 0) 
		{

			//this is data
			BingoData *data = (BingoData*)buffer;
			if (data->type == BingoDataType::PLAY)
			{
				game->_player_id = data->player_id;
			}
			else if (data->type == BingoDataType::TURN)
			{
				if (game->_player_id == data->player_id)
				{
					game->_in_turn = true;
				}
			}
			else if (data->type == BingoDataType::RESULT)
			{
				if (game->_player_id == data->player_id)
				{
					if (data->param.result == 1)
					{
						printf("Bingo!\n");
					}
				}
				else
				{
					if (data->param.result == 1)
					{
						printf("You Lose!\n");
					}
				}
			}

			//printf("Data received.\n");
			game->_waiting_data = false;
		}
		else
		{
			//this is view
			//do show here
			char view[BOARD_SIDE * BOARD_SIDE];
			strncpy(view, buffer, BOARD_SIDE * BOARD_SIDE);
			for (int y = 0; y < BOARD_SIDE; y++)
			{
				for (int x = 0; x < BOARD_SIDE; x++)
				{
					printf("%c ", view[x + BOARD_SIDE * y]);
				}
				printf("\n");
			}

			//printf("View received.\n");
			game->_waiting_data = false;
		}
	}

	return 0;
}