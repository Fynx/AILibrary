#ifndef STATE_H
#define STATE_H

#include <array>
#include <unordered_map>

struct State {
	State(int v = 0);

	static inline int get_mask(int m);

	int & operator=(int v);

	void move(int m);
	void undo(int m);
	bool can_move(int m);

	int value;
};

inline int State::get_mask(int m)
{
	return (1 << m);
}

struct Point {
	Point() {};
	Point(int x, int y) : x(x), y(y) {}

	inline Point & operator+=(Point p);

	int x, y;
};

Point & Point::operator+=(Point p)
{
	x += p.x;
	y += p.y;
	return *this;
}

namespace Direction {
	const int RR = 0;
	const int UR = 1;
	const int UU = 2;
	const int UL = 3;
	const int LL = 4;
	const int DL = 5;
	const int DD = 6;
	const int DR = 7;

	const std::array <int, 8> all {
		RR,
		UR,
		UU,
		UL,
		LL,
		DL,
		DD,
		DR,
	};

	const int RR_M = State::get_mask(RR);
	const int UR_M = State::get_mask(UR);
	const int UU_M = State::get_mask(UU);
	const int UL_M = State::get_mask(UL);
	const int LL_M = State::get_mask(LL);
	const int DL_M = State::get_mask(DL);
	const int DD_M = State::get_mask(DD);
	const int DR_M = State::get_mask(DR);

	const std::unordered_map <int, Point> change {
		{RR, { 1,  0}},
		{UR, { 1,  1}},
		{UU, { 0,  1}},
		{UL, {-1,  1}},
		{LL, {-1,  0}},
		{DL, {-1, -1}},
		{DD, { 0, -1}},
		{DR, { 1, -1}},
	};

	inline int reversed(int dir)
	{
		return (dir + 4) % 8;
	}
};

#endif // STATE_H
