#pragma once
#include "defines.h"
#include "renderer.h"
#include "text.h"
#include "timer.h"
// #include "level_editor.h"
namespace def {
	struct LevelEditor;
	// Console color 120,130,50.

	const int MAX_EDITABLE_STRING_SIZE = 200;
	const float MULTIBACKSPACE_TIME    = 0.7;
	const float MULTI_DELETE_TIME      = 0.05;

	struct EditableString{
		// This string uses unicode data, NOT utf8.
		int cursor = 0;
		unsigned int data[200] = {};
	};

	void add_unicode_to_string(EditableString *e_string, unsigned int unicode);
	void backspace_string(EditableString *e_string);
	void clear_string(EditableString *string);

	struct Console{
		RECT bounding_box;
		RECT text_box_divider;
		VEC_3D color = {120, 130, 50};
		VEC_3D division_color;
		Font *font = NULL;
		bool show = false;
		
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
	void toggle_console(Console *console);
	void render_console(Console *console, Renderer *renderer);
	void console_out(Console *console, CHR_STR_CON message);
}