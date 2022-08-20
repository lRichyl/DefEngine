#include "console.h"
#include "game.h"
#include "utilities.h"

const int CONSOLE_HEIGHT = 300;
const int BUFFER_SIZE    = 100;
const char LEVELS_PATH[] = "assets/levels/";
const char EXTENSION[]   = ".lvl";

void init_console(Console *console, Renderer *renderer){
	console->font = get_font(&Game::asset_manager, "console_font");
	console->bounding_box     = {0, renderer->window->internalHeight, renderer->window->internalWidth, CONSOLE_HEIGHT};
	console->text_box_divider = {0, console->bounding_box.y - (CONSOLE_HEIGHT - console->text_box_divider_height * 2 )+ console->font->size, renderer->window->internalWidth, console->text_box_divider_height};
	console->division_color   = {console->color.x - console->color.x * 0.5, console->color.y - console->color.y * 0.5, console->color.z - console->color.z * 0.5};
	
	init_array(&console->buffer, &Game::main_arena, BUFFER_SIZE);
	// Configure the console shader and its view matrix.
	console->shader = get_shader(&Game::asset_manager, "Console_shader");
	glUseProgram(console->shader.id);
	
	glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	int view_uniform_id = glGetUniformLocation(console->shader.id, ("u_view"));
	glUniformMatrix4fv(view_uniform_id, 1, GL_FALSE, glm::value_ptr(view));
	
	glUseProgram(0);
	
	console->backspace_timer.timeToWait    = MULTIBACKSPACE_TIME;
	console->multi_delete_timer.timeToWait = MULTI_DELETE_TIME;
}

void update_console(Console *console, LevelEditor *editor, Renderer *renderer){
	if(!is_key_being_pressed(renderer->window, GLFW_KEY_ENTER)){
		for(int i = 0; i < Input::unicode_array.size; i++){
			add_unicode_to_string(&console->string, Input::unicode_array[i]);
		}
		
		static bool delete_first = true;
		static bool multi_delete = false;
		if(is_key_being_pressed(renderer->window, GLFW_KEY_BACKSPACE)){
			console->backspace_timer.Tick();
			if(delete_first){
				backspace_string(&console->string);
				delete_first = false;
			}
			else if(console->backspace_timer.isTimeReached) multi_delete = true;
			if(multi_delete){
				console->multi_delete_timer.Tick();
				if(console->multi_delete_timer.isTimeReached) backspace_string(&console->string);
			}
		}
		else{
			// printf("NOT PRESSED\n");
			console->backspace_timer.Reset();
			console->multi_delete_timer.Reset();
			delete_first = true;
			multi_delete = false;
		}
	}
	else{
		char *string = unicode_array_to_string(&console->string, &Game::main_arena);
		char first[50] {}; // This is the command. 
		char second[50]{}; // This is the command's argument.
		get_first_two_words(string, first, second);
		int word_count = get_word_count(string);
		if(word_count == 2){
				
			if(!strcmp(first, "save_new")){
				strcpy(editor->edited_level.name, second);
				char level_path[50] = {};
				strcat(level_path, LEVELS_PATH);
				strcat(level_path, second);
				strcat(level_path, EXTENSION);

				if(editor->state == EditorState::EDITOR_TEST){
					console_out(console, "Cannot save a level while it is being played.");
				}
				else if(save_new_level(level_path))
					console_out(console, "Succesfully saved.");
				else 
					console_out(console, "A level with that name already exists.");
			}
			else if(!strcmp(first, "load")){
				char level_path[50] = {};
				strcat(level_path, LEVELS_PATH);
				strcat(level_path, second);
				strcat(level_path, EXTENSION);
				if(editor->state == EditorState::EDITOR_TEST){
					console_out(console, "Cannot load a level while it is being played.");
				}
				else if(load_level_in_editor(level_path)){
					console_out(console, "Level loaded successfully");
				}
				else
					console_out(console, "Level could not be loaded. Possibly doesn't exist.");
			}
		}
		else if (word_count == 1){
			if(!strcmp(first, "save")){
				char level_path[50] = {};

				if(editor->state == EditorState::EDITOR_TEST){
					console_out(console, "Cannot save a level while it is being played.");
				}
				else if(!editor->edited_level.name){
					console_out(console, "There's currently no level loaded.");
					// console->string.cursor = 0;
					// return;
				}
				else{
					strcat(level_path, LEVELS_PATH);
					strcat(level_path, editor->edited_level.name);
					strcat(level_path, EXTENSION);
					if(save_level(level_path)){
						console_out(console, "Level succesfully overwritten.");
					}
					else console_out(console, "The current level is not saved. Use save_new LEVEL_NAME");					
				}
			}
			else if(!strcmp(first, "hi")){
				add_array(&console->buffer, "Hello!");
			}
		}
		free_from_arena(&Game::main_arena, string);
		clear_string(&console->string);
	}
}

void render_console(Console *console, Renderer *renderer){
	render_queue_colored_rect(&Game::layers_render_commands[LEVEL_LAYERS - 1], renderer, &console->bounding_box, console->color, 255, &console->shader);
	render_queue_colored_rect(&Game::layers_render_commands[LEVEL_LAYERS - 1], renderer, &console->text_box_divider, console->division_color, 255, &console->shader);
	static V2 text_position = {console->bounding_box.x, console->bounding_box.y - console->bounding_box.h};
	render_queue_text(&Game::layers_render_commands[LEVEL_LAYERS - 1], renderer, console->font, &console->string, text_position, {255,255,255}, false, &console->shader);
	
	static V2 buffer_origin_position =  {console->bounding_box.x, console->bounding_box.y - console->bounding_box.h + console->font->size + console->text_box_divider.h * 2};
	for(int i = 0; i < console->buffer.size; i++){
		V2 position = buffer_origin_position;
		position.y += i * console->font->size;
		render_queue_text(&Game::layers_render_commands[LEVEL_LAYERS - 1], renderer, console->font, console->buffer[console->buffer.size - 1 - i], position, {255,255,255}, false, &console->shader);
	}
}

void toggle_console(Console *console){
	console->show = !console->show;
	if(console->show){
		clear_string(&console->string);
	}
}

void console_out(Console *console, const char *message){
	add_array(&console->buffer, message);
}

void add_unicode_to_string(EditableString *e_string, unsigned int unicode){
	// const char *s = string;
	if(e_string->cursor < MAX_EDITABLE_STRING_SIZE){
		e_string->data[e_string->cursor] = unicode;
		e_string->cursor++;
	}
}

void backspace_string(EditableString *e_string){
	if(e_string->cursor > 0){
		e_string->cursor--;
		// e_string->data[e_string->cursor] = 0;
	}
}

void clear_string(EditableString *string){
	for(int i = 0; i < string->cursor; i++){
		string->data[i] = 0;
	}
	string->cursor = 0;
}