#ifndef TEXT_H
#define TEXT_H
#include "renderer.h"
#include "math.h"
#include "stb_truetype.h"
#include "glad/glad.h"
#include <vector>
#include <map>
#include <string>


struct CharacterInfo{
	Rect clipping_box;
	float width;
	float height;
	float down_padding;
};

struct Font{
	Font(const char *path, float size);
	static const int texture_size = 512;
	float size;
	
	Texture texture;
	stbtt_bakedchar characters_data[96];
	std::map<char, CharacterInfo> characters;
};

void render_text(Renderer* renderer, Font *font, const char *text, V2 position, V3 color = {1.0f, 1.0f, 1.0f}
               , bool center = false);

// struct Text{
//      void SetGlyphs();
//      void Render(Renderer *renderer, std::string text);
//
//      int x;
//      int y;
//      float size;
//      std::string currentText;
//      Texture font;
//
//      std::vector<Rect> glyphsToRender {};
//      std::vector<Rect> glyphsPositions {};
//
//      static std::map<std::string, Rect> glyphsMap;
//
// };

#endif
