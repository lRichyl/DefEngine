#pragma once
#include "memory_arena.h"

template<typename T>
struct DefArray{
	T &operator[](int i){
		if(i >= capacity){
			printf("Array out of bounds\n");
			exit(-1);
		}
		return data[i];
	}
	int capacity = 0;
	int size = 0;
	T *data;
};

template<typename T>
inline T array_at(DefArray<T> *array, int i){
	return array->data[i];
}

// template<typename T>
// inline T* ptr_array_at(DefArray<T> *array, int i){
	// return array->data[i];
// }

template<typename T>
inline void init_array(DefArray<T> *array, MemoryArena *arena, int capacity){
	array->capacity = capacity;
	array->data = allocate_array_from_arena<T>(arena, capacity);
}

template<typename T>
inline void add_array(DefArray<T> *array, T data){
	if(array->size >= array->capacity - 1){
		printf("Array full\n");
		return;
	}
	array->data[array->size] = data;
	array->size++;
	
}

template<typename T>
void erase_from_array(DefArray<T> *array, int index){
	for(int i = index; i < array->size - 1; i++){
		array->data[i] = array_at(array, i + 1);
	}
	array->size--;
}


template<typename T>
inline void clear_array(DefArray<T> *array){
	array->size = 0;
}

// Returns a pointer to the last element added.
template<typename T>
inline T* get_last_element(DefArray<T> *array){
	return &array->data[array->size - 1];
}
