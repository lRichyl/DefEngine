#pragma once 
// #include <unordered_map>
#include "defines.h"
#include "def_table.h"
#include "text.h"

#include "renderer.h"
// #include "texture.h"

namespace def{
	struct AssetManager{
		// Save only textures for the moment.
		// std::unordered_map<CHR_STR_CON , Texture>       textures;
		// std::unordered_map<CHR_STR_CON , ShaderProgram> shaders;
		// std::unordered_map<CHR_STR_CON , Font>          fonts;
		DefTable<Texture>       *textures;
		DefTable<ShaderProgram> *shaders;
		DefTable<Font>          *fonts;
	};

	void init_asset_manager(AssetManager *m);
	void add_texture(AssetManager *m, CHR_STR_CON name, CHR_STR_CON path);
	Texture get_texture(AssetManager *m, CHR_STR_CON name);
	Texture* get_texture_ptr(AssetManager *m, CHR_STR_CON name);
	Texture get_empty_texture();
	void add_shader(AssetManager *m, Renderer *renderer, CHR_STR_CON name, CHR_STR_CON path);
	ShaderProgram get_shader(AssetManager *m, CHR_STR_CON name);
	ShaderProgram* get_shader_ptr(AssetManager *m, CHR_STR_CON name);
	void add_font(AssetManager *m, CHR_STR_CON name, CHR_STR_CON path, int size);
	Font* get_font(AssetManager *m, CHR_STR_CON name);
}