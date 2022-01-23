#ifndef TEXT_H
#define TEXT_H
#include "renderer.h"
#include "stb_truetype.h"
#include "glad/glad.h"
#include <unordered_map>

static const int FIRST_CHARACTER      = 32;
static const int LAST_CHARACTER       = 255;
static const int AMOUNT_OF_CHARACTERS = LAST_CHARACTER - FIRST_CHARACTER;
static const int SPACE                = 15;


struct CharacterInfo{
	Rect clipping_box;
	float width;
	float height;
	float down_padding;
	float advance;
};

struct Font{
	Font(const char *path, float size);
	static const int texture_size = 512;
	float size;
	
	Texture texture;
	stbtt_bakedchar characters_data[AMOUNT_OF_CHARACTERS];
	std::unordered_map<char, CharacterInfo> characters;
};

int utf8_to_unicode(unsigned short c);
void render_text(Renderer* renderer, Font *font, const char *text, V2 position, V3 color = {1.0f, 1.0f, 1.0f}
               , bool center = false);



#endif
