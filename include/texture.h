#pragma once
#include <cstddef>
#include "memory_arena.h"
#include "glad/glad.h"
#include "stb_image.h"
#include "def_math.h"
#include <assert.h>

namespace def {
     struct Texture{
          unsigned int id = -1;
          int width;
          int height;
          int channels;
     	 int size;
          unsigned char *data_buffer = NULL;
     };

     Texture make_texture(const char *path);
     Texture make_texture(unsigned int channels, int width, int height);
     void set_pixel(Texture *texture, V4 color, V2 location);
     V4 get_pixel(Texture *texture, V2 location);
     void update_texture(Texture *texture);
     void clear_texture(Texture *texture);
}