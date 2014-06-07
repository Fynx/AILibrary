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

	int depth = get_depth();

	if (move_counter < 5)
		depth -= 4;

	alpha_beta_recursive(0, height, true, depth - 3);

	alpha_beta_recursive(current_field.y, current_field.y + 2, true, depth);

	data = transposition_table[current_hash % TRANSPOSITION_TABLE_SIZE];
	if (data.value < data.alpha || data.value > data.beta)
		alpha_beta_with_nega_scout(0, height, true, depth);

	data = transposition_table[current_hash % TRANSPOSITION_TABLE_SIZE];
	int best_move = data.best_move;

	if (best_move == -1 || !move_possible(best_move))
		best_move = any_move();

	make_move(best_move);

	return best_move;
}

int AlphaBeta::get_depth()
{
	return MAX_DEPTH;
}

bool AlphaBeta::player_changes(int d)
{
	return field(current_field + Direction::change.at(d)).value == 0;
}

static inline int max(int a, int b)
{
	return a > b ? a : b;
}

static inline int min(int a, int b)
{
	return a < b ? a : b;
}

int AlphaBeta::alpha_beta_with_nega_scout(int alpha, int beta, bool player, int depth)
{
	if (current_field.y == 0 || current_field.y == height - 1 || depth == 0)
		return field_value(current_field);

	Data data = transposition_table[current_hash % TRANSPOSITION_TABLE_SIZE];
	if (data.hash == current_hash && data.precision >= depth - 1 &&
	  ((data.value <= data.beta && data.value >= data.alpha) ||
	   (data.value > data.beta  && data.beta >= beta) ||
	   (data.value < data.alpha && data.alpha <= alpha)))
		return data.value;

	int x;
	bool change;

	int r    = (int) !player * INF;
	int best = data.best_move;

	if (best >= 0 && move_possible(best)) {
		change = player_changes(best);

		make_move(best);
		r = alpha_beta_recursive(alpha * (int) player + beta * (int) !player,
		                         alpha * (int) player + beta * (int) !player, player ^ change, depth - 1);
		undo_move(best);

		if (player) {
			if (r >= beta) {
				transposition_table[current_hash % TRANSPOSITION_TABLE_SIZE] =
					Data(r, alpha, beta, best, depth, current_hash);
				return r;
			}
		} else {
			if (r <= alpha) {
				transposition_table[current_hash % TRANSPOSITION_TABLE_SIZE] =
					Data(r, alpha, beta, best, depth, current_hash);
				return r;
			}
		}
	}

	if (player) {
		for (int d : Direction::all) {
			if (move_possible(d)) {
				change = player_changes(d);

				make_move(d);
				x = alpha_beta_with_nega_scout(max(r, alpha), beta, player ^ change, depth - 1);
				undo_move(d);

				if (x >= beta) {
					transposition_table[current_hash % TRANSPOSITION_TABLE_SIZE] =
						Data(x, alpha, beta, d, depth, current_hash);
					return x;
				}

				if (x > r) {
					r = x;
					best = d;
				}
			}
		}
	} else {
		for (int d : Direction::all) {
			if (move_possible(d)) {
				change = player_changes(d);

				make_move(d);
				x = alpha_beta_with_nega_scout(alpha, min(r, beta), player ^ change, depth - 1);
				undo_move(d);

				if (x <= alpha) {
					transposition_table[current_hash % TRANSPOSITION_TABLE_SIZE] =
						Data(x, alpha, beta, d, depth, current_hash);
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

int AlphaBeta::alpha_beta_recursive(int alpha, int beta, bool player, int depth)
{
	if (current_field.y == 0 || current_field.y == height - 1 || depth == 0)
		return field_value(current_field);

	Data data = transposition_table[current_hash % TRANSPOSITION_TABLE_SIZE];
	if (data.hash == current_hash && data.precision >= depth - 1 &&
	    ((data.value <= data.beta && data.value >= data.alpha) ||
	     (data.value > data.beta  && data.beta >= beta) ||
	     (data.value < data.alpha && data.alpha <= alpha)))
		return data.value;

	int x;
	bool change;

	int r    = (int) !player * INF;
	int best = data.best_move;

	if (best >= 0 && move_possible(best)) {
		change = player_changes(best);

		make_move(best);
		r = alpha_beta_recursive(alpha, beta, player ^ change, depth - 1);
		undo_move(best);

		if (player) {
			if (r >= beta) {
				transposition_table[current_hash % TRANSPOSITION_TABLE_SIZE] =
					Data(r, alpha, beta, best, depth, current_hash);
				return r;
			}
		} else {
			if (r <= alpha) {
				transposition_table[current_hash % TRANSPOSITION_TABLE_SIZE] =
					Data(r, alpha, beta, best, depth, current_hash);
				return r;
			}
		}
	}

	if (player) {
		for (int d : Direction::all) {
			if (move_possible(d)) {
				change = player_changes(d);

				make_move(d);
				x = alpha_beta_recursive(max(r, alpha), beta, player ^ change, depth - 1);
				undo_move(d);

				if (x >= beta) {
					transposition_table[current_hash % TRANSPOSITION_TABLE_SIZE] =
						Data(x, alpha, beta, d, depth, current_hash);
					return x;
				}

				if (x > r) {
					r = x;
					best = d;
				}
			}
		}
	} else {
		for (int d : Direction::all) {
			if (move_possible(d)) {
				change = player_changes(d);

				make_move(d);
				x = alpha_beta_recursive(alpha, min(r, beta), player ^ change, depth - 1);
				undo_move(d);

				if (x <= alpha) {
					transposition_table[current_hash % TRANSPOSITION_TABLE_SIZE] =
						Data(x, alpha, beta, d, depth, current_hash);
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

int AlphaBeta::any_move()
{
	int d = current_hash % 8;

	for (int i = d; i < 8; ++i)
		if (move_possible(i))
			return i;
	for (int i = 0; i < d; ++i)
		if (move_possible(i))
			return i;
	return -1;
}

Hash AlphaBeta::get_hash_zobrist(int d, const Point &p)
{
	return hash_board[(p.y * width + p.x) * 8 + d];
}

void AlphaBeta::clear_table()
{
	for (int i = 0; i < TRANSPOSITION_TABLE_SIZE; ++i)
		transposition_table[i] = Data();
}
