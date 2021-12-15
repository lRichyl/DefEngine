#pragma once 
#include <unordered_map>
#include <string>

#include "renderer.h"

struct AssetManager{
	// Save only textures for the moment.
	std::unordered_map<std::string, Texture> textures;
};

void add_texture(AssetManager *m, const std::string &name, Texture texture);
Texture get_texture(AssetManager *m, const std::string &texture_name);