#pragma once
#include "renderer.h"
#include "text.h"
// #include "level_editor.h"
struct LevelEditor;
// Console color 120,130,50.

const int MAX_EDITABLE_STRING_SIZE = 200;
const float MULTIBACKSPACE_TIME    = 0.7;
const float MULTI_DELETE_TIME      = 0.05;

struct EditableString{
	// This string uses unicode data NOT utf8.
	int cursor = 0;
	unsigned int data[200] = {};
};

void add_unicode_to_string(EditableString *e_string, unsigned int unicode);
void backspace_string(EditableString *e_string);
void clear_string(EditableString *string);

struct Console{
	Rect bounding_box;
	Rect text_box_divider;
	V3 color = {120, 130, 50};
	V3 division_color;
	Font *font = NULL;
	bool show_console = false;
	
	// We render the console with its own shader to set individually its MVP so it will not scale and move
	// when we move the camera.
	ShaderProgram shader;
	float scroll_offset = 0;
	float text_box_divider_height = 10;
	
	DefArray<const char*> buffer;
	EditableString string;
	Timer backspace_timer;
	Timer multi_delete_timer;
};

void init_console(Console *console, Renderer *renderer);
void update_console(Console *console, LevelEditor *editor, Renderer *renderer);
void render_console(Console *console, Renderer *renderer);