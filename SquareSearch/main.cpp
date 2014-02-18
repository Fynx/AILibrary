#include <iostream>
#include "SquareSearch.h"

int main()
{
	uint32_t length;
	uint32_t alph_size;
	uint32_t tries_per_level;
	std::cout << "Insert length of text and size of an alphabet "
	             "(if size > 32 there will be some funny characters).\n";
	std::cin >> length >> alph_size;
	std::cout << "Insert number of trials per level.\n";
	std::cin >> tries_per_level;
	
	std::cout << "\nNMCS\n";
	SquareNMCS qs(length, alph_size, std::string(), tries_per_level);
	std::cout << qs.get_best_string() << "\n"
	          << "result: "
	          << qs.get_square_number(qs.get_best_string())
	          << "\n";

	std::cout << "\nBrute force\n";
	SquareBFSearch bs(length, alph_size);
	std::cout << bs.get_best_string() << "\n"
	          << "result: "
	          << bs.get_square_number(bs.get_best_string())
	          << "\n\n";
}