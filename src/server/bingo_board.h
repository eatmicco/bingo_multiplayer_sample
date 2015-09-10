#ifndef BINGO_BOARD
#define BINGO_BOARD

#include <vector>

struct BingoItem
{
	int id;
	int x;
	int y;
};

class BingoBoard {
private:
	int _side;
	int _turn;
	std::vector<std::vector<BingoItem>> _players;
	std::vector<char> _player_representations;
	unsigned char *_view;
public:
	BingoBoard(int s);
	~BingoBoard();

	int add_new_player(char representation); //returns the newly added player Id
	void put_item_to_board(int x, int y, int playerId);
	bool check_bingo(int &player_id); //returns true if bingo. if not bingo, player id returns -1
	void create_view(char *view);
	int get_side();
	int get_player_count();
	void set_turn(int turn);
};

#endif //BINGO_BOARD