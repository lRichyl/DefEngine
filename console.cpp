#include "console.h"
#include "game.h"

#define CONSOLE_HEIGHT 300
// #define TEXT_BOX_MARGIN 10

void init_console(Console *console, Renderer *renderer){
	console->font = get_font(&Game::asset_manager, "console_font");
	console->bounding_box     = {0, renderer->window->internalHeight, renderer->window->internalWidth, CONSOLE_HEIGHT};
	console->text_box_divider = {0, console->bounding_box.y - (CONSOLE_HEIGHT - console->text_box_divider_height * 2 )+ console->font->size, renderer->window->internalWidth, console->text_box_divider_height};
	console->division_color   = {console->color.x - console->color.x * 0.5, console->color.y - console->color.y * 0.5, console->color.z - console->color.z * 0.5};
	
	// Configure the console shader and its view matrix.
	console->shader = get_shader(&Game::asset_manager, "Console_shader");
	glUseProgram(console->shader.id);
	
	glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	int view_uniform_id = glGetUniformLocation(console->shader.id, ("u_view"));
	glUniformMatrix4fv(view_uniform_id, 1, GL_FALSE, glm::value_ptr(view));
	
	
	
	glUseProgram(0);

}

void update_console(Console *console){
	
}

void render_console(Console *console, Renderer *renderer){
	render_colored_rect(renderer, &console->bounding_box, console->color, 255, &console->shader);
	render_colored_rect(renderer, &console->text_box_divider, console->division_color, 255, &console->shader);
}