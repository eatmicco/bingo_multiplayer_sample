#ifndef BINGO_GAME_H
#define BINGO_GAME_H

#include "client_network.h"

class BingoGame
{
private:
	ClientNetwork *_network;
	HANDLE _bg_thread;
	int _player_id;
	bool _finish;
	bool _waiting_data;
	bool _in_turn;

	void get_command(char *buffer, int len);
	void query_command();
	void show();
	void add_new_player(char icon);
	void put_to_board(unsigned int player_id, unsigned short x, unsigned short y);

	static unsigned int _stdcall receiving_data(void *param);
public:
	BingoGame();

	void run();
};

#endif //BINGO_GAME_H