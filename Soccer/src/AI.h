#ifndef AI_H
#define AI_H

#include <vector>
#include "State.h"

class AI
{
public:
	AI();

	void set_board_size(int w, int h);
	void set_time_left(int t);
	void make_move(int move);
	virtual int get_move();

protected:
	State & field(Point point);
	State & field(int x, int y);

	int time_left;

	int width;
	int height;

	Point current_field;

	std::vector <State> fields;
};

#endif // AI_H
