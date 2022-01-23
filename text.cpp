#include "text.h"
#include "renderer.h"
#include "string.h"
#include <cmath>



static int BUFFER_SIZE          = 1 << 20;
static int FIRST_CHARACTER      = 32;
static int AMOUNT_OF_CHARACTERS = 96;
static int LAST_CHARACTER       = FIRST_CHARACTER + AMOUNT_OF_CHARACTERS;
static int SPACE                = 15;

Font::Font(const char *path, float size){
	texture.width = texture_size;
	texture.height  = texture_size;
	this->size = size;
	unsigned char ttf_buffer[BUFFER_SIZE];
	unsigned char temp_bitmap[texture_size*texture_size];
	unsigned int ftex;
	 
	FILE *file = fopen(path, "rb");
	
	if(!file){
		printf("File does not exist\n");
		exit(1);
	}
	fread(ttf_buffer, 1, BUFFER_SIZE, file);
	stbtt_BakeFontBitmap(ttf_buffer,0, size, temp_bitmap,texture_size,texture_size, FIRST_CHARACTER,AMOUNT_OF_CHARACTERS, characters_data); 
	glGenTextures(1, &ftex);
	glBindTexture(GL_TEXTURE_2D, ftex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, texture_size,texture_size, 0, GL_RED, GL_UNSIGNED_BYTE, (void*)temp_bitmap);
	GLint swizzleMask[] = {GL_ONE, GL_ONE, GL_ONE, GL_RED};
	glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	texture.id = ftex;
	glBindTexture(GL_TEXTURE_2D, 0);
	fclose(file);
	 
	// Cache the character's clipping box in the texture, width and height.
	float x = 0;
	float y = 0;
	for(int i = 0; i < AMOUNT_OF_CHARACTERS; ++i){
		stbtt_aligned_quad q;
		
		stbtt_GetBakedQuad(characters_data, texture_size,texture_size, i, &x, &y ,&q,1);
		
		int width  = abs(q.x1 - q.x0);
		int height = abs(q.y1 - q.y0);
		int clip_y_pos = texture_size - (q.t0 * texture_size) - height - 1;
		Rect clipping_box = {q.s0 * texture_size, clip_y_pos, (q.s1 - q.s0) * texture_size, (q.t1 - q.t0) * texture_size};
		
		CharacterInfo char_info;
		char_info.clipping_box = clipping_box;
		char_info.width        = width;
		char_info.height       = height;
		char_info.down_padding = q.y1;
		
		// Add every character to the map.
		characters[i] = char_info;
		
	}
	// We set the space's character width manually because we use the character's width to position the characters but the sbtt_GetBakedQuad
	// returns a width of 0 for the space character.
	characters[0].width = SPACE;
}

void render_text(Renderer* renderer, Font *font, const char *text, V2 position, V3 color, bool center){
     int length = strlen(text);
	 
	 //This does not center correctly, it's just a very raw approximation.
     // if(center){
		// float textSize = length * font->size;
		// printf("%f\n", textSize);
		// finalPosition.x = position.x - textSize/4.0f;//This is a hack. We should store the characters data to calculate this properly.
		// finalPosition.y = position.y;
     // }
     for(int i = 0; i < length; ++i){
		int char_index = text[i] - 32;
		CharacterInfo *character = &font->characters[char_index];
		// Rect boundingBox = {q.x0, finalPosition.y * 2 - q.y0  - font->size, (q.x1 - q.x0), (q.y1 - q.y0)}; // Left this here in case I need it later.
		Rect bounding_box = {position.x, position.y + character->height - character->down_padding, character->width, character->height};
		position.x += character->width;
		
		render_quad(renderer, &bounding_box, &font->texture, 0, &character->clipping_box, false, 255, color, true);
     }
	// once = false;

}

