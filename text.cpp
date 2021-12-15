#include "text.h"
#include "renderer.h"
// #include "string.h"


// TODO: Store all the character data in a map when the font is constucted.
Font::Font(const char *path, float size){
     texture.width = texture_size;
     texture.height  = texture_size;
     this->size = size;
     unsigned char ttf_buffer[1<<15];
     unsigned char temp_bitmap[texture_size*texture_size];
     unsigned int ftex;

     fread(ttf_buffer, 1, 1<<15, fopen(path, "rb"));
     stbtt_BakeFontBitmap(ttf_buffer,0, size, temp_bitmap,texture_size,texture_size, 32,96, characters_data); 
     // can free ttf_buffer at this point
     glGenTextures(1, &ftex);
     glBindTexture(GL_TEXTURE_2D, ftex);
     glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, texture_size,texture_size, 0, GL_RED, GL_UNSIGNED_BYTE, temp_bitmap);
     GLint swizzleMask[] = {GL_ONE, GL_ONE, GL_ONE, GL_RED};
     glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
     // can free temp_bitmap at this point
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
     texture.id = ftex;
     // glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}

void render_text(Renderer* renderer, Font *font, std::string *text, V2 position, V3 color, bool center){
     // while (*text < 128) {
     int texture_size = font->texture_size;
     int length = text->length();
     V2 finalPosition = position;
	 
	 //This does not center correctly, it's just a very raw approximation.
     if(center){
          float textSize = length * font->size;
          // printf("%f\n", textSize);
          finalPosition.x = position.x - textSize/4.0f;//This is a hack. We should store the characters data to calculate this properly.
          finalPosition.y = position.y;
     }
     for(int i = 0; i < length; ++i){
          stbtt_aligned_quad q;
          stbtt_GetBakedQuad(font->characters_data, texture_size,texture_size, (*text)[i]-32, &finalPosition.x,&finalPosition.y,&q,1);

          Rect boundingBox = {q.x0, (finalPosition.y - q.y0) + finalPosition.y - font->size/1.5f  , (q.x1 - q.x0), (q.y1 - q.y0)};

          Rect clippingBox = {q.s0 * texture_size, q.t0 * texture_size, (q.s1 - q.s0) * texture_size, (q.t1 - q.t0) * texture_size};
          render_quad(renderer, &boundingBox, &font->texture, 0, &clippingBox, false, 255, color, true);
     }

}

