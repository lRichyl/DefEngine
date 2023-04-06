#pragma once
#include "defines.h"
#include "renderer.h"
#include "stb_truetype.h"
#include "glad/glad.h"
#include <unordered_map>

namespace def {
	struct EditableString;
	static const int FIRST_CHARACTER      = 32;
	// We should change this if we want to support more characters. Don't forget to increase the texture size
	// to fit all the characters.
	static const int LAST_CHARACTER       = 255;
	static const int AMOUNT_OF_CHARACTERS = LAST_CHARACTER - FIRST_CHARACTER;
	static const int SPACE                = 15;


	struct CharacterInfo{
		RECT clipping_box;
		float width;
		float height;
		float down_padding;
		float advance;
	};

	struct Font{
		Font(){};
		Font(CHR_STR_CON path, float size);
		static const int texture_size = 512;
		float size;
		
		Texture texture;
		stbtt_bakedchar characters_data[AMOUNT_OF_CHARACTERS];
		std::unordered_map<char, CharacterInfo> characters;
	};

	int utf8_to_unicode(unsigned short c);
	void render_text(Renderer* renderer, Font *font, CHR_STR_CON text, VEC_2D position, VEC_3D color = {255, 255, 255}
	               , bool center = false, ShaderProgram *shader = NULL);
	void render_queue_text(DefArray<RenderCommand> *commands, Renderer* renderer, Font *font, CHR_STR_CON text, VEC_2D position, VEC_3D color = {255, 255, 255}
	               , bool center = false, ShaderProgram *shader = NULL);

	void render_text(Renderer* renderer, Font *font, EditableString *string, VEC_2D position, VEC_3D color = {255, 255, 255}
	               , bool center = false, ShaderProgram *shader = NULL);
	void render_queue_text(DefArray<RenderCommand> *commands, Renderer* renderer, Font *font, EditableString *string, VEC_2D position, VEC_3D color = {255, 255, 255}
	               , bool center = false, ShaderProgram *shader = NULL);
	float get_text_width(Font *font, CHR_STR_CON text);		
	char *unicode_array_to_string(EditableString *e_string, MemoryArena *arena);

}