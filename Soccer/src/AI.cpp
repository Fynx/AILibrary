#include "AI.h"

#include <iostream>

AI::AI()
{}

void AI::set_board_size(int w, int h)
{
	w += 1;
	h += 3;

	width  = w;
	height = h;

	fields.resize(w * h + 1);

	/** Side walls */
	int wall_up    = Direction::UU_M | Direction::UL_M | Direction::UR_M;
	int wall_down  = Direction::DD_M | Direction::DL_M | Direction::DR_M;
	int wall_left  = Direction::LL_M | Direction::UL_M | Direction::DL_M;
	int wall_right = Direction::RR_M | Direction::DR_M | Direction::UR_M;
	int wall_vertical   = Direction::UU_M | Direction::DD_M;
	int wall_horizontal = Direction::LL_M | Direction::RR_M;

	int left_wall  = wall_left  | wall_vertical;
	int right_wall = wall_right | wall_vertical;
	int upper_wall = wall_up    | wall_horizontal;
	int lower_wall = wall_down  | wall_horizontal;

	for (int i = 0; i < w; ++i) {
		if (i < w / 2 - 1 || i > w / 2 + 1) {
			field(i, 1)     = lower_wall;
			field(i, h - 2) = upper_wall;
		}
	}

	for (int i = 1; i < h - 1; ++i) {
		field(0, i)     = left_wall;
		field(w - 1, i) = right_wall;
	}

	field(w / 2 - 1, 1)     = Direction::LL_M | Direction::DL_M | Direction::DD_M;
	field(w / 2 + 1, 1)     = Direction::RR_M | Direction::DR_M | Direction::DD_M;
	field(w / 2 - 1, h - 2) = Direction::LL_M | Direction::UL_M | Direction::UU_M;
	field(w / 2 + 1, h - 2) = Direction::RR_M | Direction::UR_M | Direction::UU_M;


	current_field = Point(w / 2, h / 2);

	/** Debug */

	std::cerr << "Board:\n";
	for (int i = h - 1; i >= 0; --i) {
		for (int j = 0; j < w; ++j) {
			if (current_field.x == j && current_field.y == i) {
				std::cerr << "  * ";
			} else {
				if (field(j, i).value < 100) {
					std::cerr << " ";
					if (field(j, i).value < 10)
						std::cerr << " ";
				}
				std::cerr << field(j, i).value << " ";
			}
		}
		std::cerr << "\n";
	}
	std::cerr << "\n";
}

void AI::set_time_left(int t)
{
	time_left = t;
}

void AI::make_move(int move)
{
	field(current_field).move(move);
	current_field += Direction::change.at(move);
	field(current_field).move(Direction::reversed(move));
}

int AI::get_move()
{
	return Direction::UU;
}

State & AI::field(Point point)
{
	return fields[point.y * width + point.x];
}

State & AI::field(int x, int y)
{
	return fields[y * width + x];
}


