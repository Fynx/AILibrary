#include <set>
#include "SquareSearch.h"

/**
 * \class SquareSearch
 */

SquareSearch::SquareSearch(uint32_t length, uint8_t alph_size, const std::string &text)
	: length(length), alph_size(alph_size), text(text)
{
	init_powers_array();
}

SquareSearch::~SquareSearch()
{}

uint32_t SquareSearch::get_square_number(const std::string &text) const
{
	if (text.size() == 0)
		return 0;

	uint32_t length = text.size();

	uint64_t t[length + 1];
	t[0] = 0;

	for (size_t index = 1; index <= length; ++index)
		t[index] = t[index - 1] * (alph_size + 1) + text[index - 1] % alph_size + 1;

	std::set<uint64_t> set;
	uint64_t h1, h2;

	for (size_t i = 1; i <= length; ++i) {
		for (size_t j = 1; i + j * 2 - 1 <= length; ++j) {
			h1 = t[i + j - 1] - t[i - 1] * pows[j];
			h2 = t[i + j * 2 - 1] - t[i + j - 1] * pows[j];

			if (h1 == h2)
				set.insert(h1);
		}
	}

	return set.size();
}

void SquareSearch::init_powers_array()
{
	pows.resize(length);
	pows[0] = 1;
	for (uint64_t num = 1; num < length; ++num)
		pows[num] = pows[num - 1] * (alph_size + 1);
}

/**
 * \class SquareNMCS
 */

SquareNMCS::SquareNMCS(uint32_t length, uint8_t alph_size, const std::string &text, uint32_t tries_per_level)
	: SquareSearch(length, alph_size, text)
{
	this->tries_per_level = tries_per_level;
	init_random();
}

std::string SquareNMCS::get_best_string()
{
	if (text.size() == length)
		return text;

	while (text.size() < length)
		text += get_next_letter();

	return text;
}

void SquareNMCS::init_random()
{
	srand(time(NULL));
}

char SquareNMCS::get_next_letter()
{
	if (text.size() == 0)
		return char(ALPH_BEGIN);

	uint32_t best[MAX_ALPH_SIZE];
	for (uint8_t index = 0; index < MAX_ALPH_SIZE; ++index)
		best[index] = 0;

	for (uint32_t index = 0; index < tries_per_level; ++index) {
		char c = ALPH_BEGIN + rand() % alph_size;
		best[uint8_t(c)] = std::max(best[uint8_t(c)], sample(text + c));
	}

	uint8_t result = ALPH_BEGIN;
	for (uint8_t index = 0; index < MAX_ALPH_SIZE; ++index)
		if (best[result] < best[index])
			result = index;
	return char(result);
}

uint32_t SquareNMCS::sample(const std::string &text)
{
	if (length < text.size()) {
		std::cerr << "critical error\n";
		exit(0);
	}

	std::string suffix;
	while (suffix.size() < length - text.size())
		suffix += ALPH_BEGIN + rand() % alph_size;

	return get_square_number(text + suffix);
}

/**
 * \class SquareBFSearch
 */

SquareBFSearch::SquareBFSearch(uint32_t length, uint8_t alph_size, const std::string &text)
	: SquareSearch(length, alph_size, text)
{
	if (alph_size > 1 && length > MAX_LENGTH) {
		std::cerr << "critical error: too big value\n";
		exit(0);
	}
}

std::string SquareBFSearch::get_best_string()
{
	text = std::string();
	for (uint32_t index = 0; index < length; ++index)
		text += char(ALPH_BEGIN);

	std::string best = text;
	uint32_t best_res = get_square_number(best);
	uint32_t index;
	uint32_t cand;

	do {
		for (index = length; index > 0 && text[index - 1] == ALPH_BEGIN + alph_size - 1; --index)
			text[index - 1] = ALPH_BEGIN;
		if (index > 0) {
			cand = get_square_number(text);
			if (cand > best_res) {
				best_res = cand;
				best = text;
			}
			++text[index - 1];
		}
	} while (index > 0);

	return best;
}