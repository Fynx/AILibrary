#ifndef ALPHABETA_H
#define ALPHABETA_H

#include "AI.h"

class AlphaBeta : public AI
{
public:
	AlphaBeta();

	virtual int get_move();

private:
	static const int INF       = 10000;
	static const int MAX_DEPTH = 9;

	bool player_changes(int d);
	int field_value(const Point &field);
	int alpha_beta(int alpha, int beta, bool player, int depth);
};

#endif // ALPHABETA_H
