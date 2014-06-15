#ifndef STATE_H
#define STATE_H

#include <array>
#include <unordered_map>
#include <iostream>

/**
 * \struct State
 */

struct State {
	State(int v = 0) : value(v) {}

	static inline int get_mask(int m);

	inline void operator=(int v);
	inline void operator|=(int v);

	inline void move(int m);
	inline void undo(int m);
	inline bool can_move(int m) const;
	inline bool is_full() const;

	int value;
};

inline int State::get_mask(int m)
{
	return (1 << m);
}

inline void State::operator=(int v)
{
	value = v;
}

inline void State::operator|=(int v)
{
	value |= v;
}

inline void State::move(int m)
{
	value |= get_mask(m);
}

inline void State::undo(int m)
{
	value &= (~get_mask(m));
}

inline bool State::can_move(int m) const
{
	return !(value & get_mask(m));
}

inline bool State::is_full() const
{
	return (value & 255) == 255;
}

/**
 * \struct Point
 */

struct Point {
	Point() {};
	Point(int8_t x, int8_t y) : x(x), y(y) {}

	inline Point & operator+=(Point p);
	inline Point operator+(Point p);

	int8_t x, y;
};

Point & Point::operator+=(Point p)
{
	x += p.x;
	y += p.y;
	return *this;
}

Point Point::operator+(Point p)
{
	return {x + p.x, y + p.y};
}

/**
 * \namespace Direction
 */

namespace Direction {
	const int8_t RR = 0;
	const int8_t UR = 1;
	const int8_t UU = 2;
	const int8_t UL = 3;
	const int8_t LL = 4;
	const int8_t DL = 5;
	const int8_t DD = 6;
	const int8_t DR = 7;

	const std::array <int8_t, 8> all {
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

	const std::unordered_map <int8_t, Point> change {
		{RR, { 1,  0}},
		{UR, { 1,  1}},
		{UU, { 0,  1}},
		{UL, {-1,  1}},
		{LL, {-1,  0}},
		{DL, {-1, -1}},
		{DD, { 0, -1}},
		{DR, { 1, -1}},
	};

	inline int8_t reversed(int8_t dir)
	{
		return (dir + 4) % 8;
	}
};

#endif // STATE_H
