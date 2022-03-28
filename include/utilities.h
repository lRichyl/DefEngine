#pragma once

#include "stdio.h"
#include "stdlib.h"
#include "game.h"
#include <string>

char* text_file_to_char(const char* file_name);

template<typename T>
const char *to_string(T number){
	std::string s = std::to_string(number);
	char *result = allocate_array_from_arena<char>(&Game::main_arena, 5);
	const char *string = s.c_str();
	for(int i = 0; i < 5; i++){
		result[i] = string[i] ;
	}
	
	return result;
}

int check_if_file_exists(const char * filename);
int get_word_count(const char *string);
void get_first_two_words(const char *string, char *first, char *second);
