#include "DangerousAlphaBeta.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <limits>

DangerousAlphaBeta::DangerousAlphaBeta()
{
        transposition_table = new Data[TRANSPOSITION_TABLE_SIZE];
}

DangerousAlphaBeta::~DangerousAlphaBeta()
{
        delete[] transposition_table;
}

void DangerousAlphaBeta::set_board_size(int w, int h)
{
        AI::set_board_size(w, h);

        std::srand(std::time(NULL));
        hash_board.resize(width * height * 8 + 1);

        current_hash = 0;

        for (DangerousHash &v : hash_board) {
                v = std::rand() * std::numeric_limits<int>::max() + std::rand();
                current_hash ^= v;
        }
}

void DangerousAlphaBeta::make_move(int move)
{
        current_hash ^= get_hash_zobrist(move, current_field);
        AI::make_move(move);
        current_hash ^= get_hash_zobrist(Direction::reversed(move), current_field);
}

void DangerousAlphaBeta::undo_move(int move)
{
        current_hash ^= get_hash_zobrist(Direction::reversed(move), current_field);
        AI::undo_move(move);
        current_hash ^= get_hash_zobrist(move, current_field);
}

int DangerousAlphaBeta::get_move()
{
        static int move_counter = 1;

        /** Clean the transposition table */

        if (!(move_counter++ % 4))
                clear_table();

	alpha_beta_recursive(0, height, true, MAX_DEPTH);

        Data data = transposition_table[current_hash % TRANSPOSITION_TABLE_SIZE];
        int best_move = data.best_move;

        if (best_move == -1)
                best_move = any_move();

        make_move(best_move);

        return best_move;
}

bool DangerousAlphaBeta::player_changes(int d)
{
        return field(current_field + Direction::change.at(d)).value == 0;
}

int DangerousAlphaBeta::alpha_beta_recursive(int alpha, int beta, bool player, int depth)
{
        if (current_field.y == 0 || current_field.y == height - 1 || depth == 0)
                return field_value(current_field);

        Data data = transposition_table[current_hash % TRANSPOSITION_TABLE_SIZE];
        if (data.precision >= depth)
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

int DangerousAlphaBeta::any_move()
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

DangerousHash DangerousAlphaBeta::get_hash_zobrist(int d, const Point &p)
{
        return hash_board[(p.y * width + p.x) * 8 + d];
}

void DangerousAlphaBeta::clear_table()
{
        for (int i = 0; i < TRANSPOSITION_TABLE_SIZE; ++i)
                insert_data(transposition_table[i], 0, INF, -1, -1, -1);
}