#ifndef ALPHABETA_H
#define ALPHABETA_H

#include "AI.h"
#include <vector>

typedef unsigned long long Hash;

class AlphaBeta : public AI
{
public:
	AlphaBeta();
	~AlphaBeta();

	virtual void set_board_size(int w, int h);

	virtual void make_move(int move);
	virtual void undo_move(int move);
	virtual int get_move();

private:
	static const int INF                      = 63;
	static const int MAX_DEPTH                = 18;
	static const int TRANSPOSITION_TABLE_SIZE = (1 << 25);

	int get_depth();

	bool player_changes(int d);
	int field_value(const Point &field);

	int alpha_beta_with_nega_scout(int alpha, int beta, bool player, int depth);
	int alpha_beta_recursive(int alpha, int beta, bool player, int depth);
	int any_move();

	Hash current_hash;

	std::vector <Hash> hash_board;

	Hash get_hash_zobrist(int d, const Point &p);

	struct Data {
		Data() :
			value(-1), alpha(INF), beta(0), best_move(-1), precision(-1), hash(0) {}
		Data(int value, int8_t alpha, int8_t beta, int8_t best_move, int8_t p, Hash hash) :
			value(value), alpha(alpha), beta(beta), best_move(best_move), precision(p), hash(hash) {}
		Data(const Data &d) :
			value(d.value), alpha(d.alpha), beta(d.beta), best_move(d.best_move), precision(d.precision), hash(d.hash) {}

		int value;
		int8_t alpha;
		int8_t beta;
		int8_t best_move;
		int8_t precision;
		Hash hash;
	};
	Data *transposition_table;

	void clear_table();
};

inline int AlphaBeta::field_value(const Point &field)
{
	return field.y;
}

#endif // ALPHABETA_H