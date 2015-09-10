#include "bingo_board.h"
#include <stdio.h>

BingoBoard::BingoBoard(int s)
	: _side(s), _turn(0)
{
	int size = _side * _side;
	_view = new unsigned char[size];
	for (int i = 0; i < size; i++)
	{
		_view[i] = 0;
	}
}

BingoBoard::~BingoBoard()
{
	auto itr = _players.begin();
	for (; itr != _players.end(); itr++) {
		(*itr).clear();
	}
	_players.clear();
}

//returns the newly added player Id
int BingoBoard::add_new_player(char representation)
{
	std::vector<BingoItem> new_player_items;
	_players.push_back(new_player_items);
	_player_representations.push_back(representation);

	return _players.size();
}

void BingoBoard::put_item_to_board(int x, int y, int player_id)
{
	static int number = 0;

	if (_view[x + _side*y] == 0) {
		BingoItem item;
		item.id = number;
		item.x = x;
		item.y = y;
		_players[player_id - 1].push_back(item);
		number++;

		_view[x + _side*y] = player_id;
	}
}

bool BingoBoard::check_bingo(int &player_id)
{
	//check vertical
	int item = 0;
	int count = 0;
	for (int x = 0; x < _side; x++)
	{
		for (int y = 0; y < _side; y++)		
		{
			int idx = x + _side*y;
			if (_view[idx] == 0) continue;

			if (_view[idx] != item)
			{
				item = _view[idx];
				count = 1;
			}
			else
			{
				count++;
			}
			if (count == _side) 
			{
				player_id = item;
				return true;
			}
		}
	}

	//check horizontal
	item = 0, count = 0;
	for (int y = 0; y < _side; y++)		
	{
		for (int x = 0; x < _side; x++)
		{
			int idx = x + _side*y;
			if (_view[idx] == 0) continue;

			if (_view[idx] != item)
			{
				item = _view[idx];
				count = 1;
			}
			else
			{
				count++;
			}
			if (count == _side) 
			{
				player_id = item;
				return true;
			}
		}
	}

	//check diagonal TL to BR
	item = 0, count = 0;
	for (int x = 0, y = 0; x < _side; x++, y++)
	{
		int idx = x + _side*y;
		if (_view[idx] == 0) continue;

		if (_view[idx] != item)
		{
			item = _view[idx];
			count = 1;
		}
		else
		{
			count++;
		}
		if (count == _side) 
		{
			player_id = item;
			return true;
		}
	}

	//check diagonal BL to TR
	item = 0, count = 0;
	for (int x = 0, y = _side-1; x < _side; x++, y--)
	{
		int idx = x + _side*y;
		if (_view[idx] == 0) continue;

		if (_view[idx] != item)
		{
			item = _view[x + _side*y];
			count = 1;
		}
		else
		{
			count++;
		}
		if (count == _side) 
		{
			player_id = item;
			return true;
		}
	}

	player_id = -1;
	return false;
}

void BingoBoard::create_view(char *view)
{
	for (int y = 0; y < _side; y++)
	{
		for (int x = 0; x < _side; x++)
		{
			int id = x + _side * y;
			if (_view[id] != 0)
			{
				view[id] = _player_representations[(_view[id])-1];
			}
			else
			{
				view[id] = '_';
			}
		}
	}
}

int BingoBoard::get_side()
{
	return _side;
}

int BingoBoard::get_player_count()
{
	return _players.size();
}

void BingoBoard::set_turn(int turn)
{
	_turn = turn;
}
