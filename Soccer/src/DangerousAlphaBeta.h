#ifndef DANGEROUSALPHABETA_H
#define DANGEROUSALPHABETA_H

#include "AI.h"
#include <vector>

typedef unsigned int DangerousHash;

/**
 * This a dangerous version of AlphaBeta that in fact, should not be used.
 * It doesn't have any hash checking, doesn't care for any precision bounds
 * in transposition table.
 * However, it DOES work quite good and it might not if ANYTHING is changed.
 * So, it exists here as a class depriving this project of any objective thinking,
 * but, well, it works.
 */
class DangerousAlphaBeta : public AI
{
public:
        DangerousAlphaBeta();
        ~DangerousAlphaBeta();

        virtual void set_board_size(int w, int h);

        virtual void make_move(int move);
        virtual void undo_move(int move);
        virtual int get_move();

private:
        static const int INF                      = 10000;
        static const int MAX_DEPTH                = 15;
        static const int TRANSPOSITION_TABLE_SIZE = 33554432;

        bool player_changes(int d);
        int field_value(const Point &field);

        int alpha_beta_recursive(int alpha, int beta, bool player, int depth);
        int any_move();

        DangerousHash current_hash;

        std::vector <DangerousHash> hash_board;

        DangerousHash get_hash_zobrist(int d, const Point &p);

	/**
	 * Don't be fooled by the complicated data stored in this structure.
	 * What is REALLY used is value, best_move and precision. Done.
	 */
        struct Data {
                Data() :
                        alpha(INF), beta(0), value(-1), best_move(-1), precision(-1) {}
                Data(int alpha, int beta, int value, int best_move, int8_t p) :
                        alpha(alpha), beta(beta), value(0), best_move(best_move), precision(p) {}

                int alpha;
                int beta;
                int value;
                int best_move;
                int8_t precision;
        };
        Data *transposition_table;

        void clear_table();
        static inline void insert_data(Data &data, int alpha, int beta, int value, int best_move, int precision);
};

inline int DangerousAlphaBeta::field_value(const Point &field)
{
	return field.y;
}

/** Why the hell I need this...? */
inline void DangerousAlphaBeta::insert_data(Data &data, int alpha, int beta, int value, int best_move, int precision)
{
        data.alpha = alpha;
        data.beta  = beta;
        data.value = value;
        data.best_move = best_move;
        data.precision = precision;
}

#endif // DANGEROUSALPHABETA_H