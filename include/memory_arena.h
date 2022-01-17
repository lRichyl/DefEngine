#pragma once
#include <assert.h>
#include <cstdlib>
#include <forward_list>
#include <utility>
#include <stdint.h>
#include <stdio.h>
#include <vector>

struct ArenaFreeInfo{
	// ArenaFreeInfo(void *data, uint32_t s){
		// data_pointer = data;
		// size = s;
	// }
	
	void *data_pointer;
	uint32_t size;
};

struct ArenaAllocatedInfo{
	void *previous;
	uint32_t size;
};

struct MemoryArena{
	void *memory;
	
	void *start;
	void *end;
	void *current;
	void *previous;
	
	uint32_t allocated_memory = 0;
	uint32_t total_memory;
	uint32_t available_memory;
	
	ArenaFreeInfo free_info;
	std::vector<ArenaFreeInfo> free_list;
	std::forward_list<ArenaAllocatedInfo> allocated_list;
};

// inline void init_memory_arena(MemoryArena *arena, uint32_t size_in_bytes);
// inline void* get_fittable_memory(MemoryArena *arena, uint32_t size);
// inline bool belongs_to_arena(MemoryArena *arena, void *address);
// inline void print_arena_info(MemoryArena *arena);

inline void init_memory_arena(MemoryArena *arena, uint32_t size_in_bytes){
	arena->memory           = new char[size_in_bytes]{};
	arena->total_memory     = size_in_bytes;
	arena->available_memory = size_in_bytes;
	// printf("%X\n", arena->memory);
	assert(arena->memory);
	arena->start            = arena->memory;
	arena->current          = arena->start;
	
	arena->end = (char*)arena->start + size_in_bytes;
	
	arena->free_list.push_back({arena->current, size_in_bytes});
}

inline void* get_fittable_memory(MemoryArena *arena, uint32_t size){
	void *result;
	for(auto it = arena->free_list.begin(); it != arena->free_list.end(); it++){
		if(size <= it->size){
			// printf("Size: %d\n", it->size);
			it->size        -= size;
			result           = it->data_pointer;
			it->data_pointer = (char*)it->data_pointer + size;
			return result;
		}
	}
	return NULL;
}

inline bool belongs_to_arena(MemoryArena *arena, void *address){
	if(address >= arena->start && address < arena->end){
		return true;
	}
	return false;
}

inline void print_arena_info(MemoryArena *arena){
	printf("\nTotal memory: %d bytes\n", arena->total_memory);
	printf("Allocated memory: %d bytes\n", arena->allocated_memory);
	printf("Available memory: %d bytes\n", arena->available_memory);
}


template<typename T>
T* allocate_from_arena(MemoryArena *arena){
	// allocated = (T*)(arena->current);
	void *fittable_memory = get_fittable_memory(arena, sizeof(T));
	if(fittable_memory){
		T* data;
		data = (T*)fittable_memory;
		// printf("Allocated Address : %X\n", fittable_memory);
		arena->allocated_memory += sizeof(T);
		arena->available_memory = arena->total_memory - arena->allocated_memory;
		
		return data;
	}
	printf("No memory left in the arena\n");
	assert(fittable_memory);
	return NULL;
	
	// printf("%X\n", allocated);
	// assert(allocated);
}

template<typename T>
T* allocate_array_from_arena(MemoryArena *arena, int array_size){
	void *fittable_memory = get_fittable_memory(arena, sizeof(T) * array_size);
	if(fittable_memory){
		T* data;
		data = (T*)fittable_memory;
		// printf("Allocated Address : %X\n", fittable_memory);
		arena->allocated_memory += sizeof(T) * array_size;
		arena->available_memory = arena->total_memory - arena->allocated_memory;
		
		return data;
	}
	printf("No memory left in the arena\n");
	assert(fittable_memory);
	return NULL;
}



template<typename T>
void free_from_arena(MemoryArena *arena, T *&allocated){
	if(!allocated) return;
	// printf("HELLO");
	if(!arena) return;
	if(belongs_to_arena(arena, allocated)){
		ArenaFreeInfo free_node = {allocated, sizeof(T)};
		arena->free_list.push_back(std::move(free_node));
		arena->available_memory += sizeof(T);
		arena->allocated_memory = arena->total_memory - arena->available_memory;
		// printf("%X\n", allocated);
		allocated = NULL;
		// printf("%X\n", allocated);
	}
}

template<typename T>
void free_array_from_arena(MemoryArena *arena, T *&allocated, int array_size){
	if(!allocated) return;
	if(!arena) return;
	if(belongs_to_arena(arena, allocated)){
		ArenaFreeInfo free_node = {allocated, sizeof(T) * array_size};
		arena->free_list.push_back(std::move(free_node));
		arena->available_memory += sizeof(T) * array_size;
		arena->allocated_memory = arena->total_memory - arena->available_memory;
		allocated = NULL;
	}
}



