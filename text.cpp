#include "text.h"
#include "renderer.h"
#include "string.h"


// TODO: Store all the character data in a map when the font is constucted.

static int BUFFER_SIZE = 1 << 20;

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
     stbtt_BakeFontBitmap(ttf_buffer,0, size, temp_bitmap,texture_size,texture_size, 32,96, characters_data); 
     // can free ttf_buffer at this point
     glGenTextures(1, &ftex);
     glBindTexture(GL_TEXTURE_2D, ftex);
     glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, texture_size,texture_size, 0, GL_RED, GL_UNSIGNED_BYTE, (void*)temp_bitmap);
     GLint swizzleMask[] = {GL_ONE, GL_ONE, GL_ONE, GL_RED};
     glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
     // can free temp_bitmap at this point
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
     texture.id = ftex;
	  glBindTexture(GL_TEXTURE_2D, 0);
     // glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	 fclose(file);
}

void render_text(Renderer* renderer, Font *font,std::string *text, V2 position, V3 color, bool center){
     // while (*text < 128) {
     int texture_size = font->texture_size;
     int length = text->length();
     // int length = strlen(text);
     V2 finalPosition = position;
	 // finalPosition.y = renderer->window->internalHeight - finalPosition.y;
	 // position.y = renderer->window->internalHeight - finalPosition.y;
	 
	 //This does not center correctly, it's just a very raw approximation.
     if(center){
          float textSize = length * font->size;
          // printf("%f\n", textSize);
          finalPosition.x = position.x - textSize/4.0f;//This is a hack. We should store the characters data to calculate this properly.
          finalPosition.y = position.y;
     }
		static bool once = true;
     for(int i = 0; i < length; ++i){
          stbtt_aligned_quad q;
		  if(once){
			printf("%f, %f, %f, %f\n", q.x0, q.y0, q.x1 - q.x0, q.y1 - q.y0);
			printf("%f, %f, %f, %f\n", q.s0, q.t0, q.s1 - q.s0, q.t1 - q.t0);
		
		}
		
          stbtt_GetBakedQuad(font->characters_data, texture_size,texture_size, (*text)[i]-32, &finalPosition.x,&finalPosition.y,&q,1);
		int width  = q.x1 - q.x0;
		int height = q.y1 - q.y0;
		int max_height;
		Rect boundingBox = {q.x0, finalPosition.y * 2 - q.y0  - font->size, (q.x1 - q.x0), (q.y1 - q.y0)};

		
		int clip_y_pos = texture_size - (q.t0 * texture_size) - height - 1;
		Rect clippingBox = {q.s0 * texture_size, clip_y_pos, (q.s1 - q.s0) * texture_size, (q.t1 - q.t0) * texture_size};
		if(once){
			printf("%f, %f, %f, %f\n", q.x0, q.y0, q.x1 - q.x0, q.y1 - q.y0);
			printf("%f, %f, %f, %f\n", q.s0, q.t0, q.s1 - q.s0, q.t1 - q.t0);
			print_rect(&clippingBox);
			print_rect(&boundingBox);
			
		}
          render_quad(renderer, &boundingBox, &font->texture, 0, &clippingBox, false, 255, color, true);
     }
	 once = false;

}

