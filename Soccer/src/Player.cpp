#include "Player.h"
#include <iostream>

static const std::string Response = std::string("=");

Player::Player()
{}

void Player::setAI(AI *ai)
{
	this->ai = ai;
}

std::string Player::board_size(int w, int h)
{
	ai->set_board_size(w, h);
	return Response;
}

std::string Player::time_left(int t)
{
	ai->set_time_left(t);
	return Response;
}

std::string Player::play(int move)
{
	ai->make_move(move);
	return Response;
}

std::string Player::gen_move()
{
	return Response + std::string(" ") + std::to_string(ai->get_move());
}

