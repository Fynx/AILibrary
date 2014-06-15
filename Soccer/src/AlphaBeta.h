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
	static const int8_t INF                      = 63;
	static const int8_t MAX_DEPTH                = 19;
	static const int    TRANSPOSITION_TABLE_SIZE = (1 << 25);

	int8_t get_depth();

	bool player_changes(int8_t d);
	int field_value(const Point &field);

	int8_t nega_scout(int8_t alpha, int8_t beta, bool player, int8_t depth);
	int8_t alpha_beta_recursive(int8_t alpha, int8_t beta, bool player, int8_t depth);
	int8_t any_move();

	Hash current_hash;

	std::vector <Hash> hash_board;

	inline Hash get_hash_zobrist(int8_t d, const Point &p);

	struct Data {
		Data() :
			value(-1), alpha(INF), beta(0), best_move(-1), precision(-1), hash(0) {}
		Data(int8_t value, int8_t alpha, int8_t beta, int8_t best_move, int8_t p, Hash hash) :
			value(value), alpha(alpha), beta(beta), best_move(best_move), precision(p), hash(hash) {}
		Data(const Data &d) :
			value(d.value), alpha(d.alpha), beta(d.beta), best_move(d.best_move), precision(d.precision), hash(d.hash) {}

		int8_t value;
		int8_t alpha;
		int8_t beta;
		int8_t best_move;
		int8_t precision;
		Hash hash;
	};
	Data *transposition_table;

	inline Data get_current_state();
	inline void set_current_state(Data &&data);

	void clear_table();
};

inline Hash AlphaBeta::get_hash_zobrist(int8_t d, const Point &p)
{
	return hash_board[(p.y * width + p.x) * 8 + d];
}

inline int AlphaBeta::field_value(const Point &field)
{
	return field.y;
}

inline AlphaBeta::Data AlphaBeta::get_current_state()
{
	Data data = transposition_table[current_hash % TRANSPOSITION_TABLE_SIZE];
	if (data.hash == current_hash)
		return data;
	else
		return Data();
}

inline void AlphaBeta::set_current_state(AlphaBeta::Data &&data)
{
	transposition_table[current_hash % TRANSPOSITION_TABLE_SIZE] = data;
}

#endif // ALPHABETA_H