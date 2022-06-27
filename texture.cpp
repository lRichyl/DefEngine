// #include "renderer.h"
#include "game.h"
#include "texture.h"
#include "memory_arena.h"
#include "glad/glad.h"
#include "stb_image.h"
#include <assert.h>
// #include "texture.h"


// TODO: Add the ability to choose the type of filter when creating the texture.
Texture make_texture(const char *path){
	Texture texture;
	texture.data_buffer = stbi_load(path, &texture.width, &texture.height, &texture.channels, 0);
	if(!texture.data_buffer){
		printf("Could not find a texture image at the relative path: %s\n", path);
		exit(-1);
	}
	 
	if(texture.channels == 4){
		glGenTextures(1, &texture.id);
		// printf("Texture ID: %d\n", texture.id);
		glBindTexture(GL_TEXTURE_2D, texture.id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width, texture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)texture.data_buffer);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
		stbi_image_free(texture.data_buffer);
	}else if(texture.channels == 3){
		glGenTextures(1, &texture.id);
		// printf("Texture ID: %d\n", texture.id);
		glBindTexture(GL_TEXTURE_2D, texture.id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture.width, texture.height, 0, GL_RGB, GL_UNSIGNED_BYTE, (void*)texture.data_buffer);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
		stbi_image_free(texture.data_buffer);
	}else{
		printf("Incorrect number of channels\n");
	}
	return texture;
}

Texture make_texture(unsigned int channels, int width, int height){
	Texture texture;
	texture.size = channels * width * height;
	// texture.data_buffer = new unsigned char[texture.size];
	texture.data_buffer = allocate_array_from_arena<unsigned char>(&Game::main_arena, texture.size);
	assert(texture.data_buffer);
	texture.width = width;
	texture.height = height;
	texture.channels = channels;
	 
	if(channels == 4){
		glGenTextures(1, &texture.id);
		// printf("Texture ID: %d\n", texture.id);
		glBindTexture(GL_TEXTURE_2D, texture.id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)texture.data_buffer);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}else if(channels == 3){
		glGenTextures(1, &texture.id);
		// printf("Texture ID: %d\n", texture.id);
		glBindTexture(GL_TEXTURE_2D, texture.id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, (void*)texture.data_buffer);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	
	assert(texture.id != -1);
	return texture;
}

// When you modify the data buffer of a texture you need to call this function to send the new data to the GPU.
// This copies the entire texture data buffer onto the GPU, so it may not be the most optimal. 
void update_texture(Texture *texture){
	glBindTexture(GL_TEXTURE_2D, texture->id);
	// glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, texture->width, texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)texture->data_buffer);
	glTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, texture->width, texture->height, GL_RGBA, GL_UNSIGNED_BYTE, (void*)texture->data_buffer);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void set_pixel(Texture *texture, V4 color, V2 location){
	int index = (location.y * texture->width + location.x) * (texture->channels);
	// printf("%d , %d\n", index, texture->size);
	assert(index <= texture->size);
	// unsigned char pixel_data[4] = {color.x, color.y, color.z, color.w} ;
	// glBindTexture(GL_TEXTURE_2D, texture->id);
	// glTexSubImage2D (GL_TEXTURE_2D, 0, location.x, location.y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, (void*)pixel_data);
	// glBindTexture(GL_TEXTURE_2D, 0);
	
	// This is done to update the texture data reference on CPU side.
	texture->data_buffer[index] = color.x;
	texture->data_buffer[index + 1] = color.y;
	texture->data_buffer[index + 2] = color.z;
	texture->data_buffer[index + 3] = color.w;
}

V4 get_pixel(Texture *texture, V2 location){
	int index = (location.y * texture->width + location.x) * (texture->channels);
	// printf("%d , %d\n", index, texture->size);
	V4 result;
	assert(index <= texture->size);
	result.x = texture->data_buffer[index];
	result.y = texture->data_buffer[index + 1];
	result.z = texture->data_buffer[index + 2];
	result.w = texture->data_buffer[index + 3];
	
	return result;
}

void clear_texture(Texture *texture){
	glBindTexture(GL_TEXTURE_2D, texture->id);
	unsigned char data[texture->size] = {}; 
	glTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, texture->width, texture->height, GL_RGBA, GL_UNSIGNED_BYTE, (void*)data);
	glBindTexture(GL_TEXTURE_2D, 0);
	memset(texture->data_buffer, 0, texture->size);
}