#include "asset_manager.h"
#include "game.h"

void init_asset_manager(AssetManager *m){
	m->textures = create_def_table<Texture>(&Game::main_arena, 100);
	m->shaders  = create_def_table<ShaderProgram>(&Game::main_arena, 100);
	m->fonts    = create_def_table<Font>(&Game::main_arena, 100);
}

void add_texture(AssetManager *m, const char *name, const char *path){
	Texture texture = make_texture(path);
	// m->textures.insert({name, texture});
	insert_to_def_table(m->textures, name, texture);
	printf("Loaded texture: %s\n", name);
}

Texture get_texture(AssetManager *m, const char *name){
	// auto result = m->textures.find(name);
	return get_from_def_table(m->textures, name);
}

Texture* get_texture_ptr(AssetManager *m, const char *name){
	// auto result = m->textures.find(name);
	return get_pointer_from_def_table(m->textures, name);
}

Texture get_empty_texture(){
	Texture texture;
	return texture;
}

void add_shader(AssetManager *m, Renderer *renderer, const char *name, const char *fragment_shader_path){
	ShaderProgram shader = make_shader(renderer, fragment_shader_path, name);
	// m->shaders.insert({name, shader});
	insert_to_def_table(m->shaders, name, shader);
}

ShaderProgram get_shader(AssetManager *m, const char *name){
	// auto result = m->shaders.find(name);
	return get_from_def_table(m->shaders, name);
}

ShaderProgram* get_shader_ptr(AssetManager *m, const char *name){
	return get_pointer_from_def_table(m->shaders, name);
}

void add_font(AssetManager *m, const char *name, const char *path, int size){
	Font font = Font(path, size);
	// m->fonts.insert({name, font});
	insert_to_def_table(m->fonts, name, font);
}
Font* get_font(AssetManager *m, const char *name){
	// auto it = (m->fonts.find(name));
	// Font *result = &((*it).second);
	return get_pointer_from_def_table(m->fonts, name);
}