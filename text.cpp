#include "text.h"
#include "renderer.h"
#include "string.h"
#include <cmath>
#include "console.h"
#include "game.h"


namespace def {
	static int BUFFER_SIZE = 1 << 20;

	Font::Font(const char *path, float size){
		texture.width = texture_size;
		texture.height  = texture_size;
		this->size = size;
		std::vector<unsigned char> ttf_buffer(BUFFER_SIZE);
		unsigned char temp_bitmap[texture_size*texture_size];
		unsigned int ftex;
		 
		FILE* file;
		fopen_s(&file, path, "rb");
		
		if(!file){
			printf("This file does not exist:\n");
			printf(path);
			exit(1);
		}
		fread(&ttf_buffer[0], 1, BUFFER_SIZE, file);
		stbtt_BakeFontBitmap(&ttf_buffer[0], 0, size, temp_bitmap, texture_size, texture_size, FIRST_CHARACTER, AMOUNT_OF_CHARACTERS, characters_data);
		glGenTextures(1, &ftex);
		glBindTexture(GL_TEXTURE_2D, ftex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, texture_size,texture_size, 0, GL_RED, GL_UNSIGNED_BYTE, (void*)temp_bitmap);
		GLint swizzleMask[] = {GL_ONE, GL_ONE, GL_ONE, GL_RED};
		glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		texture.id = ftex;
		glBindTexture(GL_TEXTURE_2D, 0);
		fclose(file);
		 
		// Cache the character's texture clipping box(position in the atlas), width and height.
		float x = 0;
		float y = 0;
		for(int i = 0; i < AMOUNT_OF_CHARACTERS; ++i){
			stbtt_aligned_quad q;
			
			stbtt_GetBakedQuad(characters_data, texture_size,texture_size, i, &x, &y ,&q,1);
			
			int width  = abs(q.x1 - q.x0);
			int height = abs(q.y1 - q.y0);
			int clip_y_pos = texture_size - (q.t0 * texture_size) - height - 1;
			Rect clipping_box = Rect(q.s0 * texture_size, clip_y_pos, (q.s1 - q.s0) * texture_size, (q.t1 - q.t0) * texture_size);
			
			CharacterInfo char_info;
			char_info.clipping_box = clipping_box;
			char_info.width        = width;
			char_info.height       = height;
			char_info.down_padding = q.y1;
			char_info.advance      = characters_data[i].xadvance;
			
			// Add every character to the map.
			characters[i] = char_info;
			
		}
		// We set the space's character width manually because we use the character's width to position the characters but the sbtt_GetBakedQuad
		// returns a width of 0 for the space character.
		characters[0].width = SPACE;
	}

	// The text origin is on the bottom left. 
	// @SPEED: For static text it's probably best to make a text type and calculate it's length when created so that we don't have
	// 		 to do it every frame. And leave this function for text that changes constantly. 
	void render_text(Renderer* renderer, Font *font, const char *text, V2 position, V3 color, bool center, ShaderProgram *shader){
	     int length = strlen(text);
		 
		if(center){
			float whole_text_size = 0;
			float tallest = 0;
			for(int i = 0; i < length; i++){
				unsigned short c = text[i];
				if(c > 127){ // If the character is greater than 127 we sample 2 bytes from the string.
					c = ((text[i] & 0x00FF) << 8) | text[i + 1] & 0x00FF;
					i++;
				}
				int char_index = utf8_to_unicode(c);
				char_index    -= 32;
				CharacterInfo *character = &font->characters[char_index];
				whole_text_size += character->advance;
				if(character->height - character->down_padding > tallest){
					tallest = character->height - character->down_padding;
				}
			}
			float middle_x_pos = whole_text_size / 2.f;
			position.x -= middle_x_pos;
			position.y -= tallest / 2.f;
		}
		
		
		for(int i = 0; i < length; i++){
			unsigned short c = text[i];
			
			// For the moment we only take into account 2 byte wide Utf-8 characters, which is enough to represent most of the 
			// alphabets.
			if(c > 127){ // If the character is greater than 127 we sample 2 bytes from the string.
				c = ((text[i] & 0x00FF) << 8) | text[i + 1] & 0x00FF;
				i++;
			}
			int char_index = utf8_to_unicode(c);
			char_index    -= 32;
			CharacterInfo *character = &font->characters[char_index];
			// Rect boundingBox = {q.x0, finalPosition.y * 2 - q.y0  - font->size, (q.x1 - q.x0), (q.y1 - q.y0)}; // Left this here in case I need it later.
			Rect bounding_box = {position.x , position.y + character->height - character->down_padding, character->width, character->height};
			position.x += character->advance;
			
			if(!shader)
				render_quad(renderer, &bounding_box, &font->texture, &character->clipping_box, false, 255, color, true);
			else
				render_quad_with_shader(renderer, &bounding_box, &font->texture, shader, &character->clipping_box, false, 255, color, true);
		}

	}
	void render_queue_text(DefArray<RenderCommand> *commands, Renderer* renderer, Font *font, const char *text, V2 position, V3 color
		, bool center, ShaderProgram *shader){
		int length = strlen(text);
		 
		if(center){
			float whole_text_size = 0;
			float tallest = 0;
			for(int i = 0; i < length; i++){
				unsigned short c = text[i];
				if(c > 127){ // If the character is greater than 127 we sample 2 bytes from the string.
					c = ((text[i] & 0x00FF) << 8) | text[i + 1] & 0x00FF;
					i++;
				}
				int char_index = utf8_to_unicode(c);
				char_index    -= 32;
				CharacterInfo *character = &font->characters[char_index];
				whole_text_size += character->advance;
				if(character->height - character->down_padding > tallest){
					tallest = character->height - character->down_padding;
				}
			}
			float middle_x_pos = whole_text_size / 2.f;
			position.x -= middle_x_pos;
			position.y -= tallest / 2.f;
		}
		
		
		for(int i = 0; i < length; i++){
			unsigned short c = text[i];
			
			// For the moment we only take into account 2 byte wide Utf-8 characters, which is enough to represent most of the 
			// alphabets.
			if(c > 127){ // If the character is greater than 127 we sample 2 bytes from the string.
				c = ((text[i] & 0x00FF) << 8) | text[i + 1] & 0x00FF;
				i++;
			}
			int char_index = utf8_to_unicode(c);
			char_index    -= 32;
			CharacterInfo *character = &font->characters[char_index];
			// Rect boundingBox = {q.x0, finalPosition.y * 2 - q.y0  - font->size, (q.x1 - q.x0), (q.y1 - q.y0)}; // Left this here in case I need it later.
			Rect bounding_box = {position.x , position.y + character->height - character->down_padding, character->width, character->height};
			position.x += character->advance;
			
			if(!shader)
				render_queue_quad(commands, renderer, &bounding_box, &font->texture, &character->clipping_box, false, 255, color, true);
			else
				render_queue_quad_with_shader(commands, renderer, &bounding_box, &font->texture, shader, &character->clipping_box, false, 255, color, true);
		}
	}

	void render_text(Renderer* renderer, Font *font, EditableString *string, V2 position, V3 color, bool center, ShaderProgram *shader){
		if(center){
			float whole_text_size = 0;
			float tallest = 0;
			for(int i = 0; i < string->cursor; i++){
				int char_index = string->data[i];
				char_index    -= 32;
				CharacterInfo *character = &font->characters[char_index];
				whole_text_size += character->advance;
				if(character->height - character->down_padding > tallest){
					tallest = character->height - character->down_padding;
				}
			}
			float middle_x_pos = whole_text_size / 2.f;
			position.x -= middle_x_pos;
			position.y -= tallest / 2.f;
		}
		
		
		for(int i = 0; i < string->cursor; i++){
			
			// For the moment we only take into account 2 byte wide Utf-8 characters, which is enough to represent most of the 
			// alphabets.
			int char_index = string->data[i];
			char_index    -= 32;
			CharacterInfo *character = &font->characters[char_index];
			// Rect boundingBox = {q.x0, finalPosition.y * 2 - q.y0  - font->size, (q.x1 - q.x0), (q.y1 - q.y0)}; // Left this here in case I need it later.
			Rect bounding_box = {position.x , position.y + character->height - character->down_padding, character->width, character->height};
			position.x += character->advance;
			
			if(!shader)
				render_quad(renderer, &bounding_box, &font->texture, &character->clipping_box, false, 255, color, true);
			else
				render_quad_with_shader(renderer, &bounding_box, &font->texture, shader, &character->clipping_box, false, 255, color, true);
		}
	}

	void render_queue_text(DefArray<RenderCommand> *commands, Renderer* renderer, Font *font, EditableString *string, V2 position, V3 color, bool center, ShaderProgram *shader){
		if(center){
			float whole_text_size = 0;
			float tallest = 0;
			for(int i = 0; i < string->cursor; i++){
				int char_index = string->data[i];
				char_index    -= 32;
				CharacterInfo *character = &font->characters[char_index];
				whole_text_size += character->advance;
				if(character->height - character->down_padding > tallest){
					tallest = character->height - character->down_padding;
				}
			}
			float middle_x_pos = whole_text_size / 2.f;
			position.x -= middle_x_pos;
			position.y -= tallest / 2.f;
		}
		
		
		for(int i = 0; i < string->cursor; i++){
			
			// For the moment we only take into account 2 byte wide Utf-8 characters, which is enough to represent most of the 
			// alphabets.
			int char_index = string->data[i];
			char_index    -= 32;
			CharacterInfo *character = &font->characters[char_index];
			// Rect boundingBox = {q.x0, finalPosition.y * 2 - q.y0  - font->size, (q.x1 - q.x0), (q.y1 - q.y0)}; // Left this here in case I need it later.
			Rect bounding_box = {position.x , position.y + character->height - character->down_padding, character->width, character->height};
			position.x += character->advance;
			
			if(!shader)
				render_queue_quad(commands, renderer, &bounding_box, &font->texture, &character->clipping_box, false, 255, color, true);
			else
				render_queue_quad_with_shader(commands, renderer, &bounding_box, &font->texture, shader, &character->clipping_box, false, 255, color, true);
		}
	}

	float get_text_width(Font *font, const char *text){
		float whole_text_size = 0;
		int length = strlen(text);
		for(int i = 0; i < length; i++){
			unsigned short c = text[i];
			if(c > 127){ // If the character is greater than 127 we sample 2 bytes from the string.
				c = ((text[i] & 0x00FF) << 8) | text[i + 1] & 0x00FF;
				i++;
			}
			int char_index = utf8_to_unicode(c);
			char_index    -= 32;
			CharacterInfo *character = &font->characters[char_index];
			whole_text_size += character->advance;
		}
		return whole_text_size;
	}

	int utf8_to_unicode(unsigned short c){
		unsigned short unicode = 0x0000;
		
		if(c < 128){
			unicode = c;
		}
		else if(c < 65536){
			unsigned char low  = c & 0x003F;
			unsigned char high = (c & 0x1F00) >> 8;
			unicode = low | (high << 6);
		}
		return unicode;
	}



	char *unicode_array_to_string(EditableString *e_string,MemoryArena *arena){
		char *string = allocate_array_from_arena<char>(arena, e_string->cursor + 1);
		for(int i = 0; i < e_string->cursor + 1; i++){
			unsigned short code_point = e_string->data[i];
			if(code_point < 128){
				string[i] = e_string->data[i];
			}
			else if(code_point < 1920){
				char byte_1 = 0b11000000;
				byte_1      = byte_1 |(code_point >> 6);
				char byte_2 = 0b10000000;
				byte_2      = byte_2 | (code_point & 0b00111111);
				
				string[i]     = byte_1;
				string[i + 1] = byte_2;
				i++;
			}
			// We do not handle 3 nor 4 byte unicode codepoints.
		}
		return string;
	}
}
