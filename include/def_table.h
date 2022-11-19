#pragma once
#include <cstring>
#include "memory_arena.h"

namespace def {
	const int MAX_CHARACTERS_FOR_HASH = 10;

	template<typename T>
	struct DefTableEntry{
		const char *key;
		T value;
	};

	// At the moment the key is a string.
	template<typename T>
	struct DefTable{
		int size;
		DefTableEntry<T> *entries;
	};

	// For now we'll use the first 10 characters of a string to compute the hash. If it has less than 10 characters then
	// all the characters are used.

	template<typename T>
	DefTable<T>* create_def_table(MemoryArena *arena, int size){
		DefTable<T> *def_table = allocate_from_arena<DefTable<T>>(arena);
		def_table->entries = allocate_array_from_arena<DefTableEntry<T>>(arena,size);
		def_table->size = size;
		
		return def_table;
	}

	inline int calculate_hash(int table_size, const char *name){
		int count = 0;
		for(int i = 0; i < MAX_CHARACTERS_FOR_HASH; i++){
			if(name[i] == '\000') break;
			count += name[i];
			
		}
		
		return count % table_size;
		
	}

	// At the moment if we try insert a value with an existing key, the current value gets replaced with the new one.
	template<typename T>
	void insert_to_def_table(DefTable<T> *table, const char *name, T value){
		int hash = calculate_hash(table->size, name);
		while(table->entries[hash].key){
			hash++;
		}
		table->entries[hash].key   = name;
		table->entries[hash].value = value;
	}

	template<typename T>
	T get_from_def_table(DefTable<T> *table, const char *key){
		int hash = calculate_hash(table->size, key);
		while( 0 != strcmp(table->entries[hash].key, key)){
			hash++;
			assert(hash < table->size);
		}
		return table->entries[hash].value;
	}

	template<typename T>
	T* get_pointer_from_def_table(DefTable<T> *table, const char *key){
		int hash = calculate_hash(table->size, key);
		const char *key_target = table->entries[hash].key;
		if(!key_target){
			printf("Texture: %s doesn't exist\n");
			return NULL;
		}
		while( 0 != strcmp(key_target, key)){
			hash++;
			if(hash >= table->size) return NULL;
		}
		
		return &table->entries[hash].value;
	}

	template<typename T>
	void delete_from_def_table(DefTable<T> *table, const char *key){
		int hash = calculate_hash(table->size, key);
		int comp = strcmp(table->entries[hash].key, key);
		while( 0 != strcmp(table->entries[hash].key, key)){
			hash++;
		}
		
		table->entries[hash].key = "";
	}
}