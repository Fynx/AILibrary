#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

class SquareSearch {
public:
	SquareSearch(uint32_t length, uint8_t alph_size, const std::string &text = std::string());
	virtual ~SquareSearch();

	virtual uint32_t get_square_number(const std::string &text) const;
	virtual std::string get_best_string() = 0;

protected:
	uint32_t length;
	uint8_t alph_size;
	std::string text;

	static const uint8_t  ALPH_BEGIN      = uint8_t('a');
	static const uint8_t  MAX_ALPH_SIZE   = uint8_t(255);

private:
	std::vector<uint64_t> pows;

	void init_powers_array();
};

class SquareNMCS : public SquareSearch {
public:
	SquareNMCS(uint32_t length, uint8_t alph_size, const std::string &text = std::string(),
	         uint32_t tries_per_level = DEFAULT_TRIES_PER_LEVEL);

	std::string get_best_string();

private:
	void init_random();
	char get_next_letter();
	uint32_t sample(const std::string &text);

	static const uint32_t DEFAULT_TRIES_PER_LEVEL = 100;
	uint32_t tries_per_level;
};

class SquareBFSearch : public SquareSearch {
public:
	SquareBFSearch(uint32_t length, uint8_t alph_size, const std::string &text = std::string());

	std::string get_best_string();

private:
	static const int MAX_LENGTH = 20;
};