#include "AlphaBeta.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <limits>

#define DEBUGMODE

#ifdef DEBUGMODE
static const int _debug = true;
#else
static const int _debug = false;
#endif

#define DEBUG if (_debug)

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
	array_hash_board.resize(width * height * 8 + 1);

	/** Double-hashing */

	current_hash = 0;

	for (Hash &v : hash_board) {
		v = (Hash) std::rand() * (Hash) std::numeric_limits<int>::max() + (Hash) std::rand();
		current_hash ^= v;
	}

	current_array_hash = 0;

	for (Hash &v : array_hash_board) {
		v = (Hash) std::rand() * (Hash) std::numeric_limits<int>::max() + (Hash) std::rand();
		current_array_hash ^= v;
	}
}

void AlphaBeta::make_move(int move)
{
	current_hash ^= get_hash_zobrist(move, current_field);
	current_array_hash ^= get_hash_zobrist(move, current_field);
	AI::make_move(move);
	current_hash ^= get_hash_zobrist(Direction::reversed(move), current_field);
	current_array_hash ^= get_hash_zobrist(Direction::reversed(move), current_field);
}

void AlphaBeta::undo_move(int move)
{
	current_hash ^= get_hash_zobrist(Direction::reversed(move), current_field);
	current_array_hash ^= get_hash_zobrist(Direction::reversed(move), current_field);
	AI::undo_move(move);
	current_hash ^= get_hash_zobrist(move, current_field);
	current_array_hash ^= get_hash_zobrist(move, current_field);
}

int AlphaBeta::get_move()
{
	/** Clean the transposition table */

	//NO CLEANING

	/** Find optimal depth depending on time left */

	int depth = get_depth_const();
DEBUG
	std::cerr << "setting depth to: " << depth << "\n";

	/** Try to find winning move */

	alpha_beta_recursive(height - 1, height - 1, true, depth);

	/** Try to find best move with the greatest number of cuts */

	alpha_beta_recursive(0, height, true, depth - 2);

	/** Try to find alpha-beta value in smaller window */

	static int window          = 3;
	static int estimated_value = (current_field.y + 2);

	alpha_beta_recursive(estimated_value - window, estimated_value + window, true, depth);

	/** If this fails, widen the window */

	Data data = get_current_data();
	if (data.value > data.beta || data.value < data.alpha)
		alpha_beta_recursive(0, height - 1, true, depth);

	/** Make the best move */

	data = get_current_data();
	int best_move = data.best_move;

	/** If every position is losing or there is a hash problem, try shallow search */

	if (best_move == -1 || best_move == 0 || data.hash != current_hash || !move_possible(best_move)) {
		clear_table();
		alpha_beta_recursive(0, height - 1, true, depth - 3);
		data = get_current_data();
		best_move = data.best_move;
	}

	if (best_move == -1 || best_move == 0 || data.hash != current_hash)
		best_move = any_move();

DEBUG
	std::cerr << "best move: " << best_move << " (value=" << data.value << ")\n";

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
	if (field(current_field).is_full())
		return (int) !player * INF;
	if (current_field.y == 0 || current_field.y == height - 1 || depth == 0)
		return field_value(current_field);

	Data data = get_current_data();
	if (data.precision >= depth && data.hash == current_hash &&
		((data.value <= data.beta && data.value >= data.alpha)
		|| (data.value >= data.beta  && data.beta >= beta)
		|| (data.value <= data.alpha && data.alpha <= alpha)))
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
				undo_move(d);

				if (x >= beta) {
					insert_data(transposition_table[current_array_hash % TRANSPOSITION_TABLE_SIZE],
					            alpha, beta, x, d, current_hash, depth);
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
					insert_data(transposition_table[current_array_hash % TRANSPOSITION_TABLE_SIZE],
					            alpha, beta, x, d, current_hash, depth);
					return x;
				}
				if (x < r) {
					r = x;
					best = d;
				}
			}
		}
	}

	insert_data(transposition_table[current_array_hash % TRANSPOSITION_TABLE_SIZE], alpha, beta, r, best, current_hash, depth);
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
DEBUG
	std::cerr << "No move possible.\n";

	return -1;
}

int AlphaBeta::get_depth_non_adaptive() const
{
	static int move_counter = 0;
	static const double CTR_DIV = 7;
	static const int AVERAGE_MOVES_NUMBER  = 80;
	static const int AVERAGE_TIME_FOR_MOVE = time_left / AVERAGE_MOVES_NUMBER;

	double ctr = 1;
	int depth = MAX_DEPTH - (int) CTR_DIV + (int) ctr;
	++move_counter;

	static const int BEGINNING_MOVES_NUMBER   = 5;

	if (move_counter > BEGINNING_MOVES_NUMBER && move_counter < AVERAGE_MOVES_NUMBER)
		while (time_left / ((double) AVERAGE_MOVES_NUMBER - (double) move_counter) >
		       AVERAGE_TIME_FOR_MOVE * ctr++ / CTR_DIV)
			++depth;

	return depth;
}

int AlphaBeta::get_depth_adaptive() const
{
	static const int AVERAGE_MOVES_NUMBER  = (width - 2) * (height - 2) * 2;
	static const int AVERAGE_TIME_FOR_MOVE = time_left / AVERAGE_MOVES_NUMBER;

DEBUG
	std::cerr << "time_left: " << time_left << "\n";

	static int move_counter      = 0;
	static int thr_previous_time = 0;
	static int sec_previous_time = 0;
	static int previous_time     = 0;
	static int time_passed       = 0;

	static int depth = MAX_DEPTH;

	++move_counter;
	time_passed += previous_time - time_left;

DEBUG
	std::cerr << "time: " << (thr_previous_time - time_left) / 3 << "/" << AVERAGE_TIME_FOR_MOVE << "\n";

	if (time_passed / move_counter > AVERAGE_TIME_FOR_MOVE ||
	    (thr_previous_time - time_left > 3 * AVERAGE_TIME_FOR_MOVE)) {
		--depth;
	} else {
		static bool checked = false;
		if (checked)
			++depth;
		checked = !checked;
	}

	thr_previous_time = sec_previous_time;
	sec_previous_time = previous_time;
	previous_time     = time_left;

	return depth;
}

int AlphaBeta::get_depth_const() const
{
	return MAX_DEPTH;
}

Hash AlphaBeta::get_hash_zobrist(int d, const Point &p)
{
	return hash_board[(p.y * width + p.x) * 8 + d];
}

void AlphaBeta::clear_table()
{
	for (int i = 0; i < TRANSPOSITION_TABLE_SIZE; ++i)
		insert_data(transposition_table[i], 0, INF, -1, -1, (Hash) 0, -1);
}