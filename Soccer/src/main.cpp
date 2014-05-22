#include <iostream>
#include <string>
#include <unistd.h>

#include "AlphaBeta.h"
#include "DangerousAlphaBeta.h"
#include "Player.h"

// #define DANGEROUS

enum class Command {
	BoardSize,
	TimeLeft,
	Play,
	GenMove,
	Invalid,
};

Command parseCommand(const std::string &cmd)
{
	if (cmd == "boardsize")
		return Command::BoardSize;
	else if (cmd == "timeleft")
		return Command::TimeLeft;
	else if (cmd == "play")
		return Command::Play;
	else if (cmd == "genmove")
		return Command::GenMove;
	else
		return Command::Invalid;
}

int main()
{
	Player player;
	AI *ai;

#ifdef DANGEROUS
	ai = new DangerousAlphaBeta();
#else
	ai = new AlphaBeta();
#endif

	player.setAI(ai);

	std::string cmd;
	std::string response;
	int arg1, arg2;

	while (true) {
		std::cin >> cmd;
		switch (parseCommand(cmd)) {
			case Command::BoardSize:
				std::cin >> arg1 >> arg2;
				response = player.board_size(arg1, arg2);
				break;
			case Command::TimeLeft:
				std::cin >> arg1;
				response = player.time_left(arg1);
				break;
			case Command::Play:
				std::cin >> arg1;
				response = player.play(arg1);
				break;
			case Command::GenMove:
				response = player.gen_move();
				break;
			case Command::Invalid:
				return EXIT_SUCCESS;
		}

		std::cout.flush();
		std::cout << response << std::endl;
	}

	return EXIT_SUCCESS;
}