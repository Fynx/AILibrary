#ifndef ALPHABETA_H
#define ALPHABETA_H

#include "AI.h"
#include <vector>

typedef uint64_t Hash;

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
	static const int INF                      = 10000;
	static const int MAX_DEPTH                = 15;
	static const int TRANSPOSITION_TABLE_SIZE = 16777216;

	bool player_changes(int d);
	int field_value(const Point &field);

	int alpha_beta_recursive(int alpha, int beta, bool player, int depth);
	int greedy_search(bool player);
	int any_move();

	int get_depth_non_adaptive() const;
	int get_depth_adaptive() const;
	int get_depth_const() const;

	Hash current_hash;
	Hash current_array_hash;

	std::vector <Hash> hash_board;
	std::vector <Hash> array_hash_board;

	Hash get_hash_zobrist(int d, const Point &p);

	struct Data {
		Data() :
			alpha(INF), beta(0), value(-1), best_move(-1), hash(0), precision(-1) {}
		Data(int alpha, int beta, int value, int best_move, Hash hash, int8_t p) :
			alpha(alpha), beta(beta), value(0), best_move(best_move), hash(hash), precision(p) {}

		int alpha;
		int beta;
		int value;
		int best_move;
		Hash hash;
		int8_t precision;
	};
	Data *transposition_table;

	void clear_table();
	inline Data get_current_data() const;
	static inline void insert_data(Data &data, int alpha, int beta, int value, int best_move, Hash hash, int precision);
};

inline AlphaBeta::Data AlphaBeta::get_current_data() const
{
	Data d = transposition_table[current_array_hash % TRANSPOSITION_TABLE_SIZE];
	if (d.hash == current_hash)
		return d;
	else
		return Data();
}

/** Why the hell I need this...? */
inline void AlphaBeta::insert_data(Data &data, int alpha, int beta, int value, int best_move, Hash hash, int precision)
{
	data.alpha = alpha;
	data.beta  = beta;
	data.value = value;
	data.best_move = best_move;
	data.hash      = hash;
	data.precision = precision;
}

#endif // ALPHABETA_H
