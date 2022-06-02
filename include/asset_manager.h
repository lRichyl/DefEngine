#pragma once 
// #include <unordered_map>
#include "def_table.h"
#include "text.h"

#include "renderer.h"
// #include "texture.h"

struct AssetManager{
	// Save only textures for the moment.
	// std::unordered_map<const char *, Texture>       textures;
	// std::unordered_map<const char *, ShaderProgram> shaders;
	// std::unordered_map<const char *, Font>          fonts;
	DefTable<Texture>       *textures;
	DefTable<ShaderProgram> *shaders;
	DefTable<Font>          *fonts;
};

void init_asset_manager(AssetManager *m);
void add_texture(AssetManager *m, const char *name, const char *path);
Texture get_texture(AssetManager *m, const char *name);
Texture* get_texture_ptr(AssetManager *m, const char *name);
Texture get_empty_texture();
void add_shader(AssetManager *m, Renderer *renderer, const char *name, const char *path);
ShaderProgram get_shader(AssetManager *m, const char *name);
ShaderProgram* get_shader_ptr(AssetManager *m, const char *name);
void add_font(AssetManager *m, const char *name, const char *path, int size);
Font* get_font(AssetManager *m, const char *name);
