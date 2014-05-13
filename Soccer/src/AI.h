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
	void undo_move(int move);
	virtual int get_move();

protected:
	inline State & field(Point point);
	inline State & field(int x, int y);

	inline bool move_possible(int d);

	int time_left;

	int width;
	int height;

	Point current_field;

	std::vector <State> fields;
};

inline State & AI::field(Point point)
{
	return fields[point.y * width + point.x];
}

inline State & AI::field(int x, int y)
{
	return fields[y * width + x];
}

inline bool AI::move_possible(int d)
{
	return field(current_field).can_move(d);
}

#endif // AI_H
