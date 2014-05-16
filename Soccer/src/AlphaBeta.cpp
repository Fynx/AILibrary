#include "AlphaBeta.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <limits>

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
		v = std::rand() * std::numeric_limits<int>::max() + std::rand();
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
	static int move_counter = 1;

	/** Clean the transposition table */

	if (!(move_counter++ % 4))
		clear_table();

	/** Try to find best move with the greatest number of cuts */

// 	alpha_beta_recursive(0, height, true, MAX_DEPTH - 2);

	/** Try to find alpha-beta value in smaller window */

// 	static int window          = 2;
// 	static int estimated_value = (current_field.y + 2) - window;

// 	alpha_beta_recursive(estimated_value - window, estimated_value + window, true, MAX_DEPTH);

	/** If this fail, widen the window */

	Data data = transposition_table[current_hash % TRANSPOSITION_TABLE_SIZE];
// 	if (data.value > data.beta || data.value < data.alpha)
		alpha_beta_recursive(0, height, true, MAX_DEPTH);

	/** Make the best move */

	data = transposition_table[current_hash % TRANSPOSITION_TABLE_SIZE];
	int best_move = data.best_move;
// 	std::cerr << "best move: " << data.best_move << " (" << data.value << ")" << std::endl;

	if (best_move == -1) {
		clear_table();
		best_move = greedy_search(true);
	}

	if (best_move == -1)
		best_move = any_move();

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

int AlphaBeta::alpha_beta_recursive(int alpha, int beta, bool player, int depth)
{
	if (current_field.y == 0 || current_field.y == height - 1 || depth == 0)
		return field_value(current_field);

	Data data = transposition_table[current_hash % TRANSPOSITION_TABLE_SIZE];
	if (data.precision >= depth)// &&
// 		((data.value <= data.beta && data.value >= data.alpha)
// 		|| (data.value >= data.beta  && data.beta >= beta)
// 		|| (data.value <= data.alpha && data.alpha <= alpha)))
		return data.value;

	int x;
	int r;
	int best = data.best_move;
	bool change;

	if (best >= 0 && move_possible(best)) {
		change = player_changes(best);

		make_move(best);
		r = alpha_beta_recursive(alpha, beta, player ^ change, depth - 1);
		undo_move(best);
	} else if (player) {
		r = 0;
	} else {
		r = INF;
	}

	if (player) {
		for (int d : Direction::all) {
			if (move_possible(d)) {
				change = player_changes(d);

				make_move(d);
				x = alpha_beta_recursive(std::max(alpha, r), beta, player ^ change, depth - 1);
// 				if (depth == MAX_DEPTH)
// 					std::cerr << "result: " << x << " (" << d << ") " << " ["
// 						<< current_field.x << ", " << current_field.y << "]" << std::endl;
				undo_move(d);

				if (x >= beta) {
					insert_data(transposition_table[current_hash % TRANSPOSITION_TABLE_SIZE], alpha, beta, x, d, depth);
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
				x = alpha_beta_recursive(alpha, std::min(beta, r), player ^ change, depth - 1);
				undo_move(d);

				if (x <= alpha) {
					insert_data(transposition_table[current_hash % TRANSPOSITION_TABLE_SIZE], alpha, beta, x, d, depth);
					return x;
				}
				if (x < r) {
					r = x;
					best = d;
				}
			}
		}
	}

	insert_data(transposition_table[current_hash % TRANSPOSITION_TABLE_SIZE], alpha, beta, r, best, depth);
	return r;
}

int AlphaBeta::greedy_search(bool player)
{
	if (current_field.y == 0)
		return -1;
	else if (current_field.y == height - 1)
		return 0;

	int x;
	bool change;

	if (player) {
		for (int d : Direction::all) {
			if (move_possible(d)) {
				change = player_changes(d);
				make_move(d);
				x = greedy_search(player ^ change);
				undo_move(d);

				if (x >= 0)
					return d;
			}
		}
		return -1;
	} else {
		for (int d : Direction::all) {
			if (move_possible(d)) {
				change = player_changes(d);
				make_move(d);
				x = greedy_search(player ^ change);
				undo_move(d);

				if (x == -1)
					return d;
			}
		}
		return 0;
	}
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
		insert_data(transposition_table[i], 0, INF, -1, -1, -1);
}