#include "asset_manager.h"


void add_texture(AssetManager *m, const std::string &name, Texture texture){
	m->textures.insert({name, texture});
}

Texture get_texture(AssetManager *m, const std::string &texture_name){
	auto result = m->textures.find(texture_name);
	return result->second;
}