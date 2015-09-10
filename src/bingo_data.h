#ifndef BINGO_DATA_H
#define BINGO_DATA_H

#define BOARD_SIDE 3

enum BingoDataType
{
	PLAY,
	TURN,
	PUT,
	RESULT,
	SHOW,
};

struct BingoData
{
	char key[5];
	unsigned char padding[3];
	unsigned int type;
	unsigned int player_id;		//32
	union {
		struct {
			unsigned short x;	//16
			unsigned short y;	//16
		} pos;
		unsigned int result;		//32
		struct {
			char icon;
			char padding[3];
		} representation;
	} param;
};

#endif //BINGO_DATA_H