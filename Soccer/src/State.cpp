#include "State.h"

State::State(int v) :
	value(v)
{}

int & State::operator=(int v)
{
	value = v;
	return value;
}

void State::move(int m)
{
	value |= get_mask(m);
}

void State::undo(int m)
{
	value -= (value & get_mask(m));
}

bool State::can_move(int m)
{
	return !(value & get_mask(m));
}

