#include "AlphaBeta.h"
#include <iostream>

AlphaBeta::AlphaBeta()
{}

int AlphaBeta::get_move()
{
	int best_move = Direction::UU;
	int best_result = 0;
	int result;
	bool change;

	for (int d : Direction::all) {
		if (move_possible(d)) {
// 			std::cerr << "checking direction: " << d << std::endl;
			change = !player_changes(d);
			make_move(d);
			result = alpha_beta(0, INF, change, MAX_DEPTH);
			undo_move(d);

			if (result >= best_result) {
				best_result = result;
				best_move   = d;
			}
		}
	}

	make_move(best_move);

	return best_move;
}

bool AlphaBeta::player_changes(int d)
{
	return field(current_field + Direction::change.at(d)).value == 0;
}

int AlphaBeta::field_value(const Point &field)
{
	return field.y;
}

int AlphaBeta::alpha_beta(int alpha, int beta, bool player, int depth)
{
	if (current_field.y == 0 || current_field.y == height - 1 || depth == 0)
		return field_value(current_field);

	int x;
	int r;
	bool change;

	if (player) {
		r = 0;
		for (int d : Direction::all) {
			if (move_possible(d)) {
				change = player_changes(d);

				make_move(d);
				x = alpha_beta(std::max(alpha, r), beta, player ^ change, depth - 1);
				undo_move(d);

				if (x >= beta)
					return x;
				r = std::max(x, r);
			}
		}
	} else {
		r = INF;
		for (int d : Direction::all) {
			if (move_possible(d)) {
				change = player_changes(d);

				make_move(d);
				x = alpha_beta(alpha, std::min(beta, r), player ^ change, depth - 1);
				undo_move(d);

				if (x <= alpha)
					return x;
				r = std::min(x, r);
			}
		}
	}

	return r;
}

