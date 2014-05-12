#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include "AI.h"

class Player
{
public:
	Player();

	void setAI(AI *ai);

	std::string board_size(int w, int h);
	std::string time_left(int t);
	std::string play(int move);
	std::string gen_move();

private:
	AI *ai;
};

#endif // PLAYER_H
