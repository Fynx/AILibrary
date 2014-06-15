#include <cstdlib>
#include <ctime>
#include <iostream>
#include <limits>

#include "AlphaBeta.h"

AlphaBeta::AlphaBeta()
{
	transposition_table = new Data[TRANSPOSITION_TABLE_SIZE];
}

AlphaBeta::~AlphaBeta()
{
	delete[] transposition_table;
}

void AlphaBeta::set_board_size(int w, int h)
{
	AI::set_board_size(w, h);

	std::srand(std::time(NULL));
	hash_board.resize(width * height * 8 + 1);

	current_hash = 0;

	for (Hash &v : hash_board) {
		v = (Hash) std::rand() * (Hash) std::numeric_limits<int>::max() + (Hash) std::rand();
		current_hash ^= v;
	}
}

void AlphaBeta::make_move(int move)
{
	current_hash ^= get_hash_zobrist(move, current_field);
	AI::make_move(move);
	current_hash ^= get_hash_zobrist(Direction::reversed(move), current_field);
}

void AlphaBeta::undo_move(int move)
{
	current_hash ^= get_hash_zobrist(Direction::reversed(move), current_field);
	AI::undo_move(move);
	current_hash ^= get_hash_zobrist(move, current_field);
}

int AlphaBeta::get_move()
{
	Data data;

	static int move_counter = 0;

	++move_counter;

	int8_t depth = get_depth();

	if (move_counter < 5)
		depth -= 4;

	nega_scout(0, height, true, depth - 3);

	nega_scout(current_field.y, current_field.y + 2, true, depth);

	data = get_current_state();
	if (data.value < data.alpha || data.value > data.beta)
		nega_scout(0, height, true, depth);

	data = get_current_state();
	int8_t best_move = data.best_move;

	if (best_move == -1 || !move_possible(best_move))
		best_move = any_move();

	make_move(best_move);

	return best_move;
}

int8_t AlphaBeta::get_depth()
{
	return MAX_DEPTH;
}

bool AlphaBeta::player_changes(int8_t d)
{
	return field(current_field + Direction::change.at(d)).value == 0;
}

static inline int8_t max(int8_t a, int8_t b)
{
	return a > b ? a : b;
}

static inline int8_t min(int8_t a, int8_t b)
{
	return a < b ? a : b;
}

int8_t AlphaBeta::nega_scout(int8_t alpha, int8_t beta, bool player, int8_t depth)
{
	if (current_field.y == 0 || current_field.y == height - 1 || depth == 0)
		return field_value(current_field);

	Data data = get_current_state();
	if (data.precision >= depth - 1 && data.value <= data.beta && data.value >= data.alpha)
		return data.value;

	int8_t x;
	bool change;

	int8_t r    = (int8_t) !player * INF;
	int8_t best = data.best_move;

	if (best >= 0 && move_possible(best)) {
		change = player_changes(best);

		make_move(best);
		r = nega_scout(
			alpha * (int8_t) !player + beta * (int8_t) player,
			alpha * (int8_t) !player + beta * (int8_t) player, player ^ change, depth - 1);
		undo_move(best);

// 		if (r >= alpha * (int8_t) !player + beta * (int8_t) player && r <= alpha * (int8_t) !player + beta * (int8_t) player) {
// 			set_current_state({r, alpha, beta, best, depth, current_hash});
// 			return r;
// 		}
		if (player) {
			if (r >= beta) {
				set_current_state({r, alpha, beta, best, depth, current_hash});
				return r;
			}
		} else {
			if (r <= alpha) {
				set_current_state({r, alpha, beta, best, depth, current_hash});
				return r;
			}
		}
	}

	if (player) {
		for (int8_t d : Direction::all) {
			if (move_possible(d)) {
				change = player_changes(d);

				make_move(d);
				x = nega_scout(max(r, alpha), beta, player ^ change, depth - 1);
				undo_move(d);

				if (x >= beta) {
					set_current_state({x, alpha, beta, d, depth, current_hash});
					return x;
				}

				if (x > r) {
					r = x;
					best = d;
				}
			}
		}
	} else {
		for (int8_t d : Direction::all) {
			if (move_possible(d)) {
				change = player_changes(d);

				make_move(d);
				x = nega_scout(alpha, min(r, beta), player ^ change, depth - 1);
				undo_move(d);

				if (x <= alpha) {
					set_current_state({x, alpha, beta, d, depth, current_hash});
					return x;
				}

				if (x < r) {
					r = x;
					best = d;
				}
			}
		}
	}

	set_current_state({r, alpha, beta, best, depth, current_hash});
	return r;
}

int8_t AlphaBeta::alpha_beta_recursive(int8_t alpha, int8_t beta, bool player, int8_t depth)
{
	if (current_field.y == 0 || current_field.y == height - 1 || depth == 0)
		return field_value(current_field);

	Data data = get_current_state();
	if (data.precision >= depth - 1 &&
	    ((data.value <= data.beta && data.value >= data.alpha) ||
	     (data.value > data.beta  && data.beta  >= beta) ||
	     (data.value < data.alpha && data.alpha <= alpha)))
		return data.value;

	int8_t x;
	bool change;

	int8_t r    = (int8_t) !player * INF;
	int8_t best = data.best_move;

	if (best >= 0 && move_possible(best)) {
		change = player_changes(best);

		make_move(best);
		r = alpha_beta_recursive(alpha, beta, player ^ change, depth - 1);
		undo_move(best);

		if (player) {
			if (r >= beta) {
				set_current_state({r, alpha, beta, best, depth, current_hash});
				return r;
			}
		} else {
			if (r <= alpha) {
				set_current_state({r, alpha, beta, best, depth, current_hash});
				return r;
			}
		}
	}

	if (player) {
		for (int8_t d : Direction::all) {
			if (move_possible(d)) {
				change = player_changes(d);

				make_move(d);
				x = alpha_beta_recursive(max(r, alpha), beta, player ^ change, depth - 1);
				undo_move(d);

				if (x >= beta) {
					set_current_state({x, alpha, beta, d, depth, current_hash});
					return x;
				}

				if (x > r) {
					r = x;
					best = d;
				}
			}
		}
	} else {
		for (int8_t d : Direction::all) {
			if (move_possible(d)) {
				change = player_changes(d);

				make_move(d);
				x = alpha_beta_recursive(alpha, min(r, beta), player ^ change, depth - 1);
				undo_move(d);

				if (x <= alpha) {
					set_current_state({x, alpha, beta, d, depth, current_hash});
					return x;
				}

				if (x < r) {
					r = x;
					best = d;
				}
			}
		}
	}

	transposition_table[current_hash % TRANSPOSITION_TABLE_SIZE] =
		Data(r, alpha, beta, best, depth, current_hash);
	return r;
}

int8_t AlphaBeta::any_move()
{
	int8_t d = current_hash % 8;

	for (int8_t i = d; i < 8; ++i)
		if (move_possible(i))
			return i;
	for (int8_t i = 0; i < d; ++i)
		if (move_possible(i))
			return i;
	return -1;
}

void AlphaBeta::clear_table()
{
	for (int i = 0; i < TRANSPOSITION_TABLE_SIZE; ++i)
		transposition_table[i] = Data();
}
