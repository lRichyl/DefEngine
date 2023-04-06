#pragma once
#include "defines.h"
#include "stdio.h"
#include "stdlib.h"
#include "game.h"
#include <string>

namespace def {
	char* text_file_to_char(const char* file_name);

	template<typename T>
	CHR_STR_CON to_string(T number){
		std::string s = std::to_string(number);
		char *result = allocate_array_from_arena<char>(&Game::main_arena, 5);
		CHR_STR_CON string = s.c_str();
		for(int i = 0; i < 5; i++){
			result[i] = string[i] ;
		}
		
		return result;
	}

	int check_if_file_exists(CHR_STR_CON  filename);
	int get_word_count(CHR_STR_CON string);
	void get_first_two_words(CHR_STR_CON string, char *first, char *second);
}