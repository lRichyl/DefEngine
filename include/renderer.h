#pragma once
#include "glad/glad.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "collision.h"
#include "window.h"
#include "math.h"
#include "texture.h"
#include "memory_arena.h"


namespace RendererInfo{
     static const int QUADS_PER_BATCH = 1000;
     static const int FLOATS_PER_QUAD = 36;
     static const int INDICES_PER_QUAD = 6;
     static const int TOTAL_INDICES =  QUADS_PER_BATCH * INDICES_PER_QUAD;
     static int MAX_TEXTURE_UNITS_PER_BATCH;
     static const int NUMBER_OF_BATCHES = 30;
}



struct ShaderProgram{
     unsigned int id = -1;
     const char *name;
};
struct Batch{
     unsigned int vbo;
     unsigned int vao;
     unsigned int texture_index = 0;
     unsigned int vertices_index = 0;
     int number_of_quads_to_copy = 0;
     int total_indices_to_draw = 0;
     unsigned int registered_textures_ids[120] = {};// This is just a hardcoded random limit
     ShaderProgram shader_program;
     // float vertex_buffer[16] = {-0.8f, 0.8f,
     //                          -0.8f, 0.0f,
     //                           0.8f, 0.0f,
     //                           0.8f, 0.8f};
     float vertex_buffer[RendererInfo::QUADS_PER_BATCH * RendererInfo::FLOATS_PER_QUAD] = {};
};
struct Renderer{
     unsigned int ibo;
     unsigned int fbo;
     unsigned int framebuffer_ibo;
     unsigned int framebuffer_vao;
     unsigned int framebuffer_vbo;
     unsigned int framebuffer_rbo;
     unsigned int framebuffer_texture;
     V2 drawing_resolution = {800, 600};
     ShaderProgram framebuffer_shader_program;
     Window *window;
     unsigned int framebuffer_index_buffer[6] = {0,1,2,2,3,0};
     float framebuffer_vertex_buffer[16] = {-1.0f, 1.0f, 0.0f, 1.0f,
                                         -1.0f, -1.0f, 0.0f, 0.0f,
                                          1.0f, -1.0f, 1.0f, 0.0f,
                                          1.0f, 1.0f, 1.0f, 1.0f};
     glm::mat4 projection;
     //Use this matrix later to move the camera.
     glm::mat4 view;
     unsigned int index_buffer[RendererInfo::TOTAL_INDICES] = {};
     int batch_index = 0;
     // Batch main_batch;
     Batch batches[RendererInfo::NUMBER_OF_BATCHES];
     // Batch ui_batch;
     Batch *current_batch;
     ShaderProgram default_shader_program;
     ShaderProgram current_shader;
	 
};






Renderer* create_renderer(Window *window);

// This function should not be called unless you want to use a different vertex shader than the default one. If you only want to 
// use a custom fragment shader use the make_shader funcion.
void compile_shader_program(ShaderProgram *shader_program, const char *vs_path, const char *fs_path);

// Call this after compiling a shader program. If not called the shader will not work.
void initialize_texture_sampler(ShaderProgram shader);

// Loads the default MVP matrix to the passed shader program's vertex shader, as long as it has two mat4 uniforms called "u_projection" and "u_view"
void load_mvp_to_shader(Renderer *renderer, ShaderProgram shader);

// Renders a textured quad in the specified position with no rotation. Usin the default shader program.
void render_quad(Renderer *renderer, Rect *position, Texture *texture, Rect *clip_region = NULL, bool mirrorX = false, float alpha_value = 255, V3 color = {255.f,255.f,255.f}, bool mirrorY = false);

// This function renders a textured quad from point A to point B. As if it was a line.
void render_quad(Renderer *renderer, V2 a, V2 b, float thickness, Texture *texture, Rect *clip_region = NULL, bool mirrorX = false, float alpha_value = 255, V3 color = {255.f,255.f,255.f}, bool mirrorY = false);

// Render a colored quad at the specified position. If no shader is provided the default one is used.
void render_colored_rect(Renderer *renderer, Rect *position, V3 color, float alpha_value = 255, ShaderProgram *shader = NULL);

// Works the same as render_quad but you can provide a different shader program.
void render_quad_with_shader(Renderer *renderer, Rect *position, Texture *texture, ShaderProgram shader, Rect *clip_region = NULL , bool mirrorX = false, float alpha_value = 255, V3 color = {255.f,255.f,255.f}, bool mirrorY = false);

// Use this to change the rendering resolution at runtime.
void change_drawing_resolution(Renderer *renderer, int width, int height);

// Creates a new shader using the default vertex shader provided a custom fragment shader.
ShaderProgram make_shader(Renderer *renderer, const char *path_to_fragment_shader, const char *name);

void renderer_draw(Renderer *renderer);
void destroy_renderer(Renderer *renderer);
void print_rect(Rect *rect);