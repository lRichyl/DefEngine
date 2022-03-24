#include "glad/glad.h"
#include "utilities.h"
#include "renderer.h"
#include "texture.h"
// #include "math.h"

#include <assert.h>


/////////// Default internal shaders ///////////////
static const char *default_vertex_shader = 
"#version 330 core \n"
"layout(location = 0) in vec2 position;\n"
"layout(location = 1) in vec2 texCoord;\n"
"layout(location = 2) in float texID;\n"
"layout(location = 3) in float alphaValue;\n"
"layout(location = 4) in vec3 color;\n"
"\n"
"out vec2 outTexCoord;\n"
"out float outTexID;\n"
"out float outAlphaValue;\n"
// "out float depth;\n"
"out vec3 outColor;\n"
"\n"
"uniform mat4 u_projection;\n"
"uniform mat4 u_view;\n"
"\n"
"void main()\n"
"{\n"
    "gl_Position = u_projection * u_view * vec4(position,1.0f, 1.0f);\n"
	// "depth = position.z;\n"
    "outTexCoord = texCoord;\n"
    "outTexID = texID;\n"
    "outAlphaValue = alphaValue;\n"
    "outColor = color;\n"
"}";

static const char *default_fragment_shader =
"#version 330 core\n"
"layout(location = 0)out vec4 fragColor;\n"
"in vec2 outTexCoord;\n"
"in float outTexID;\n"
"in float outAlphaValue;\n"
// "in float depth;\n"
"in vec3 outColor;\n"

"uniform sampler2D u_textures[32];\n"
"uniform int u_max_texture_units;\n"

"void main()\n"
"{\n"
	"switch(int(outTexID)){\n"
		"case 0: fragColor = texture2D(u_textures[0], outTexCoord); break;\n"
		"case 1: fragColor = texture2D(u_textures[1], outTexCoord); break;\n"
		"case 2: fragColor = texture2D(u_textures[2], outTexCoord); break;\n"
		"case 3: fragColor = texture2D(u_textures[3], outTexCoord); break;\n"
		"case 4: fragColor = texture2D(u_textures[4], outTexCoord); break;\n"
		"case 5: fragColor = texture2D(u_textures[5], outTexCoord); break;\n"
		"case 6: fragColor = texture2D(u_textures[6], outTexCoord); break;\n"
		"case 7: fragColor = texture2D(u_textures[7], outTexCoord); break;\n"
		"case 8: fragColor = texture2D(u_textures[8], outTexCoord); break;\n"
		"case 9: fragColor = texture2D(u_textures[9], outTexCoord); break;\n"
		"case 10: fragColor = texture2D(u_textures[10], outTexCoord); break;\n"
		"case 11: fragColor = texture2D(u_textures[11], outTexCoord); break;\n"
		"case 12: fragColor = texture2D(u_textures[12], outTexCoord); break;\n"
		"case 13: fragColor = texture2D(u_textures[13], outTexCoord); break;\n"
		"case 14: fragColor = texture2D(u_textures[14], outTexCoord); break;\n"
		"case 15: fragColor = texture2D(u_textures[15], outTexCoord); break;\n"
		"case 16: fragColor = texture2D(u_textures[16], outTexCoord); break;\n"
		"case 17: fragColor = texture2D(u_textures[17], outTexCoord); break;\n"
		"case 18: fragColor = texture2D(u_textures[18], outTexCoord); break;\n"
		"case 19: fragColor = texture2D(u_textures[19], outTexCoord); break;\n"
		"case 20: fragColor = texture2D(u_textures[20], outTexCoord); break;\n"
		"case 21: fragColor = texture2D(u_textures[21], outTexCoord); break;\n"
		"case 22: fragColor = texture2D(u_textures[22], outTexCoord); break;\n"
		"case 23: fragColor = texture2D(u_textures[23], outTexCoord); break;\n"
		"case 24: fragColor = texture2D(u_textures[24], outTexCoord); break;\n"
		"case 25: fragColor = texture2D(u_textures[25], outTexCoord); break;\n"
		"case 26: fragColor = texture2D(u_textures[26], outTexCoord); break;\n"
		"case 27: fragColor = texture2D(u_textures[27], outTexCoord); break;\n"
		"case 28: fragColor = texture2D(u_textures[28], outTexCoord); break;\n"
		"case 29: fragColor = texture2D(u_textures[29], outTexCoord); break;\n"
		"case 30: fragColor = texture2D(u_textures[30], outTexCoord); break;\n"
		"case 31: fragColor = texture2D(u_textures[31], outTexCoord); break;\n"
		"default: fragColor = vec4(outColor, 1.0f);\n"
	"}\n"
	"fragColor *= vec4(outColor, 1.0f);\n"
	"if(outAlphaValue < 1 && fragColor.w != 0){\n"
		"fragColor.w = outAlphaValue;\n"
	"}\n"
	
"}\n";

static const char *framebuffer_vertex_shader = 
"#version 330 core\n"
"layout (location = 0) in vec2 aPos;\n"
"layout (location = 1) in vec2 aTexCoords;\n"

"out vec2 TexCoords;\n"

"void main()\n"
"{\n"
    "gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0); \n"
    "TexCoords = aTexCoords;\n"
"}  \n";

static const char *framebuffer_fragment_shader =
"#version 330 core\n"
"out vec4 FragColor;\n"

"in vec2 TexCoords;\n"

"uniform sampler2D screenTexture;\n"

"void main()\n"
"{\n"
	"FragColor = texture(screenTexture, TexCoords);\n"
	
"}\n";

///////////////////////////////////////////////////////

static bool check_if_texture_is_not_registered(Texture texture, Batch *batch);
static void bind_texture(int slot, unsigned int id);
static void rebind_registered_texture_ids(Batch *batch);
static void create_shader_program(ShaderProgram *program, unsigned int vertex_shader, unsigned int fragment_shader);
static void make_vertex_shader(const char* path, unsigned int *vertex_shader);
static void make_fragment_shader(const char* path, unsigned int *fragment_shader);
static void make_vertex_shader_from_source(const char* path, unsigned int *vertex_shader);
static void make_fragment_shader_from_source(const char* path, unsigned int *fragment_shader);

// int RendererInfo::MAX_TEXTURE_UNITS_PER_BATCH = 0;

void create_framebuffer_buffers(Renderer *renderer){
     // glUseProgram(renderer->framebuffer_shader_program.id);

     glGenVertexArrays(1, &renderer->framebuffer_vao);
     glBindVertexArray(renderer->framebuffer_vao);

     glGenBuffers(1, &renderer->framebuffer_ibo);
     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->framebuffer_ibo);
     glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(renderer->framebuffer_index_buffer), (void*)renderer->framebuffer_index_buffer, GL_STATIC_DRAW);

     glGenBuffers(1, &renderer->framebuffer_vbo);
     glBindBuffer(GL_ARRAY_BUFFER, renderer->framebuffer_vbo);
     glBufferData(GL_ARRAY_BUFFER, sizeof(renderer->framebuffer_vertex_buffer), (void*)renderer->framebuffer_vertex_buffer, GL_STATIC_DRAW);

     glEnableVertexAttribArray(0);
     glVertexAttribPointer(0, 2, GL_FLOAT, false, 4 * sizeof(float), (void*)0);
     glEnableVertexAttribArray(1);
     glVertexAttribPointer(1, 2, GL_FLOAT, false, 4 * sizeof(float), (void*)( 2 * sizeof(float)));


     glBindVertexArray(0);
}

void change_drawing_resolution(Renderer *renderer, int width, int height){
	renderer->drawing_resolution.x = width;
	renderer->drawing_resolution.y = height;
	glDeleteTextures(1, &renderer->framebuffer_texture);
	glGenTextures(1, &renderer->framebuffer_texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, renderer->framebuffer_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, renderer->fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderer->framebuffer_texture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

static void initialize_framebuffer(Renderer *renderer){

     glGenFramebuffers(1, &renderer->fbo);
     glBindFramebuffer(GL_FRAMEBUFFER, renderer->fbo);

     glGenTextures(1, &renderer->framebuffer_texture);
     glActiveTexture(GL_TEXTURE0);
     glBindTexture(GL_TEXTURE_2D, renderer->framebuffer_texture);
     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (int)renderer->drawing_resolution.x, (int)renderer->drawing_resolution.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
     glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
     glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
     glBindTexture(GL_TEXTURE_2D, 0);


	 // glGenRenderbuffers(1, &renderer->framebuffer_rbo);
     // glBindRenderbuffer(GL_RENDERBUFFER, renderer->framebuffer_rbo );
     // glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, (int)renderer->drawing_resolution.x, (int)renderer->drawing_resolution.y);
	 // glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderer->framebuffer_rbo);  
	 // glBindRenderbuffer(GL_RENDERBUFFER, 0);
	 glGenTextures(1, &renderer->framebuffer_rbo);
     glBindTexture(GL_TEXTURE_2D, renderer->framebuffer_rbo);
     glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, (int)renderer->drawing_resolution.x, (int)renderer->drawing_resolution.y, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
     glBindTexture(GL_TEXTURE_2D, 0);

     glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderer->framebuffer_texture, 0);
     glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, renderer->framebuffer_rbo,0);
     GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
     glDrawBuffers(1, DrawBuffers);

     if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
          printf("ERROR: Framebuffer is not complete!");
          exit(0);
     }
     glBindFramebuffer(GL_FRAMEBUFFER, 0);

     // glUseProgram(renderer->framebuffer_shader_program.id);
     // int framebuffer_texture = glGetUniformLocation(renderer->framebuffer_shader_program.id, "screenTexture");
     
     // glBindTexture(GL_TEXTURE_2D, renderer->framebuffer_texture);
     // glUniform1i(framebuffer_texture, 0);
     // glUseProgram(0);

}

static void draw_framebuffer(Renderer *renderer){
     glUseProgram(renderer->framebuffer_shader_program.id);
     glBindVertexArray(renderer->framebuffer_vao);
     // glBindBuffer(GL_ARRAY_BUFFER, renderer->framebuffer_vbo);
     //
     // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->framebuffer_ibo);
     // glBindTexture(GL_TEXTURE_2D, renderer->framebuffer_texture);

     int framebuffer_texture = glGetUniformLocation(renderer->framebuffer_shader_program.id, "screenTexture");
     glActiveTexture(GL_TEXTURE0);
     glBindTexture(GL_TEXTURE_2D, renderer->framebuffer_texture);
     glUniform1i(framebuffer_texture, 0);

     glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
     glBindTexture(GL_TEXTURE_2D, 0);
     glBindVertexArray(0);
     glUseProgram(0);
}
static void initialize_renderer_index_buffer(Renderer *renderer){
     int indices_index = 0;
     for(int i = 0; i< RendererInfo::TOTAL_INDICES ;) {
          for(int j = 0; j< 6 ;j++, i++) {
                    if(i < 6) {
                         renderer->index_buffer[0] = 0;
                         renderer->index_buffer[1] = 1;
                         renderer->index_buffer[2] = 2;
                         renderer->index_buffer[3] = 2;
                         renderer->index_buffer[4] = 3;
                         renderer->index_buffer[5] = 0;
                        indices_index = 6;
                        i = 6;
                        break;
                    }else {
                         renderer->index_buffer[i] = renderer->index_buffer[i - 6] + 4;
                    }
                    //printf("%d ", renderer->index_buffer[i]);
          }
          //int x = test_int;
          //printf("\n");
          glGenBuffers(1, &renderer->ibo);
          glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->ibo);
          glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(renderer->index_buffer), (void*)renderer->index_buffer, GL_DYNAMIC_DRAW);
     }
}
static void initialize_batch_vertex_buffers_and_arrays(Batch *batch, Renderer *renderer){
     batch->shader_program = renderer->default_shader_program;
     glGenVertexArrays(1, &batch->vao);
     glBindVertexArray(batch->vao);
     glGenBuffers(1, &batch->vbo);
     glBindBuffer(GL_ARRAY_BUFFER, batch->vbo);
     glBufferData(GL_ARRAY_BUFFER, sizeof(batch->vertex_buffer), nullptr, GL_DYNAMIC_DRAW);

     glEnableVertexAttribArray(0);
     glVertexAttribPointer(0, 2, GL_FLOAT, false, 9 * sizeof(float), (void*)0);
     glEnableVertexAttribArray(1);
     glVertexAttribPointer(1, 2, GL_FLOAT, false, 9 * sizeof(float), (void*)( 2 * sizeof(float)));
     glEnableVertexAttribArray(2);
     glVertexAttribPointer(2, 1, GL_FLOAT, false, 9 * sizeof(float), (void*)( 4 * sizeof(float)));
     glEnableVertexAttribArray(3);
     glVertexAttribPointer(3, 1, GL_FLOAT, false, 9 * sizeof(float), (void*)( 5 * sizeof(float)));
     glEnableVertexAttribArray(4);
     glVertexAttribPointer(4, 3, GL_FLOAT, false, 9 * sizeof(float), (void*)( 6 * sizeof(float)));


     glBindVertexArray(0);
}

void initialize_texture_sampler(ShaderProgram shader){
     glUseProgram(shader.id);
     int max_texture_units;
     glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_texture_units);
     RendererInfo::MAX_TEXTURE_UNITS_PER_BATCH = max_texture_units;
     int max_texture_units_id = glGetUniformLocation(shader.id, "u_max_texture_units");
     glUniform1i(max_texture_units_id, max_texture_units);
     // printf("%d", max_texture_units);

     int texture_sampler_id = glGetUniformLocation(shader.id, "u_textures");
     int *a = new int[max_texture_units];
     for(int i = 0; i < max_texture_units; i++){
          a[i] = i;
     }
     glUniform1iv(texture_sampler_id, max_texture_units, a);
     delete a;
     glUseProgram(0);
}

void load_mvp_to_shader(Renderer *renderer, ShaderProgram shader){
     glUseProgram(shader.id);
     int projection_uniform_id = glGetUniformLocation(shader.id, ("u_projection"));
     glUniformMatrix4fv(projection_uniform_id, 1, GL_FALSE, glm::value_ptr(renderer->projection));

     int view_uniform_id = glGetUniformLocation(shader.id, ("u_view"));
     glUniformMatrix4fv(view_uniform_id, 1, GL_FALSE, glm::value_ptr(renderer->view));
     glUseProgram(0);
}



void compile_shader_program(ShaderProgram *shader_program, const char *vs_path, const char *fs_path){
     unsigned int vs;
     unsigned int fs;
     make_vertex_shader(vs_path, &vs);
     make_fragment_shader(fs_path, &fs);
     create_shader_program(shader_program, vs, fs);
     printf("SHADER PROGRAM %s\n", shader_program->name);
     printf("%s\n", vs_path);
     printf("%s\n", fs_path);
     printf("Succesfully compiled\n\n");
}

static void compile_shader_program_from_source(ShaderProgram *shader_program, const char *vs_text, const char *fs_text){
     unsigned int vs;
     unsigned int fs;
     make_vertex_shader_from_source(vs_text, &vs);
     make_fragment_shader_from_source(fs_text, &fs);
     create_shader_program(shader_program, vs, fs);
     printf("SHADER PROGRAM FROM SOURCE CODE %s\n", shader_program->name);
     // printf("%s\n", vs_text);
     // printf("%s\n", fs_path);
     printf("Succesfully compiled\n\n");
}


void initialize_renderer(Renderer *renderer, Window *window){
	// init_memory_arena(&Renderer::main_arena, 1000000); // Allocate an arena of 10MB.
     renderer->projection = glm::ortho(0.0f, (float)window->internalWidth, 0.0f, (float)window->internalHeight, 0.0f, -100.f);
     renderer->view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)); //Modify this in real time to move the camera.
     renderer->drawing_resolution.x = window->internalWidth;
     renderer->drawing_resolution.y = window->internalHeight;
     renderer->current_batch = &renderer->batches[0];

     renderer->default_shader_program.name = "Batch shader";
     renderer->framebuffer_shader_program.name = "Framebuffer shader";
     compile_shader_program_from_source(&renderer->default_shader_program, default_vertex_shader, default_fragment_shader);
     compile_shader_program_from_source(&renderer->framebuffer_shader_program, framebuffer_vertex_shader, framebuffer_fragment_shader);

     create_framebuffer_buffers(renderer);
     initialize_framebuffer(renderer);

     initialize_renderer_index_buffer(renderer);
     for(int i = 0; i < RendererInfo::NUMBER_OF_BATCHES; ++i){
          initialize_batch_vertex_buffers_and_arrays(&renderer->batches[i], renderer);
     }
     initialize_texture_sampler(renderer->default_shader_program);
     //This needs to happen after the shader program is compiled.

     load_mvp_to_shader(renderer, renderer->default_shader_program);

}

// static void re

Renderer* create_renderer(Window *window){
     Renderer *renderer = new Renderer;
     initialize_renderer(renderer, window);
     renderer->window = window;
     // printf("%d", renderer->window->width);
     return renderer;
}

void destroy_renderer(Renderer *renderer){
     glDeleteFramebuffers(1, &renderer->fbo);
     glDeleteTextures(1, &renderer->framebuffer_texture);
     delete renderer;
}

static void render_quad_on_batch(Renderer *renderer, Batch *batch, Rect *position, Texture *texture, Rect *clip_region, bool mirrorX, float alpha_value, V3 color, bool mirrorY){
     Window *win = renderer->window;
     V2 top_left_clip;
     V2 bottom_left_clip;
     V2 top_right_clip;
     V2 bottom_right_clip;
     float normalized_alpha_value = alpha_value / 255.f; // / 255.f;
	 V3 normalized_color = {color.x/255.f, color.y/255.f, color.z/255.f};
     Rect final_position;
	 // float layer  = 0;
	 // int max_layers = 100;
	 // float final_layer = (float)layer;
	 
     if(!position){
          final_position = {0.0f, (float)win->internalHeight, (float)win->internalWidth, (float)win->internalHeight};
          // printf("%f %f %f %f", final_position.x, final_position.y, final_position.w, final_position.h);
     }else{
          final_position = *position;
     }

     //The clip_region is used to select a part of a texture that we want to render.
     if(clip_region){
          assert(clip_region->w <= texture->width);
          assert(clip_region->h <= texture->height);

          top_left_clip.x     = clip_region->x / texture->width;
          top_left_clip.y     = (texture->height - clip_region->y ) / texture->height;
          bottom_left_clip.x  = clip_region->x / texture->width;
          bottom_left_clip.y  = (texture->height - clip_region->y - clip_region->h )/ texture->height;
          bottom_right_clip.x = (clip_region->x + clip_region->w) / texture->width;
          bottom_right_clip.y = (texture->height - clip_region->y - clip_region->h)/ texture->height;
          top_right_clip.x    = (clip_region->x + clip_region->w) / texture->width;
          top_right_clip.y    = (texture->height - clip_region->y) / texture->height;
     }else{
          top_left_clip.x     = 0.0f;
          top_left_clip.y     = 1.0f;
          bottom_left_clip.x  = 0.0f;
          bottom_left_clip.y  = 0.0f;
          bottom_right_clip.x = 1.0f;
          bottom_right_clip.y = 0.0f;
          top_right_clip.x    = 1.0f;
          top_right_clip.y    = 1.0f;


     }
     if(mirrorX){
          V2::SwitchXComponents(&top_left_clip, &top_right_clip);
          V2::SwitchXComponents(&bottom_left_clip, &bottom_right_clip);
     }

     if(mirrorY){
          V2::SwitchYComponents(&top_left_clip, &bottom_left_clip);
          V2::SwitchYComponents(&top_right_clip, &bottom_right_clip);
     }

     //We do this so that if we try to draw outside the window we don't add data to the vertex buffer.
     if((final_position.x + final_position.w >= 0) && (final_position.x <= win->internalWidth) && (final_position.y >= 0) && (final_position.y - final_position.h <= win->internalHeight)){
			float texture_slot_id = 0;
			if(texture){
				if(check_if_texture_is_not_registered(*texture, batch)){
				   batch->registered_textures_ids[batch->texture_index] = texture->id;
				   bind_texture(batch->texture_index, texture->id);
				   texture_slot_id = (float)batch->texture_index;
				   batch->texture_index++;
				}else{
				   for(int i = 0; i < RendererInfo::MAX_TEXTURE_UNITS_PER_BATCH; i++){
						if(texture->id == batch->registered_textures_ids[i]){
							 texture_slot_id = i;
							 break;
						}
				   }
				}
			}else{
				texture_slot_id = -1;
			}
          assert(batch->texture_index <= RendererInfo::MAX_TEXTURE_UNITS_PER_BATCH);

		// printf("%d\n", layer);
          batch->vertex_buffer[batch->vertices_index] = final_position.x;
          batch->vertex_buffer[batch->vertices_index + 1] = final_position.y;
		  // batch->vertex_buffer[batch->vertices_index + 2] = final_layer;
          batch->vertex_buffer[batch->vertices_index + 2] = top_left_clip.x;
          batch->vertex_buffer[batch->vertices_index + 3] = top_left_clip.y;
          batch->vertex_buffer[batch->vertices_index + 4] = texture_slot_id;
          batch->vertex_buffer[batch->vertices_index + 5] = normalized_alpha_value;
          batch->vertex_buffer[batch->vertices_index + 6] = normalized_color.x;
          batch->vertex_buffer[batch->vertices_index + 7] = normalized_color.y;
          batch->vertex_buffer[batch->vertices_index + 8] = normalized_color.z;

          batch->vertex_buffer[batch->vertices_index + 9] = final_position.x;
          batch->vertex_buffer[batch->vertices_index + 10] = final_position.y - final_position.h;
		  // batch->vertex_buffer[batch->vertices_index + 12] = final_layer;
          batch->vertex_buffer[batch->vertices_index + 11] = bottom_left_clip.x;
          batch->vertex_buffer[batch->vertices_index + 12] = bottom_left_clip.y;
          batch->vertex_buffer[batch->vertices_index + 13] = texture_slot_id;
          batch->vertex_buffer[batch->vertices_index + 14] = normalized_alpha_value;
          batch->vertex_buffer[batch->vertices_index + 15] = normalized_color.x;
          batch->vertex_buffer[batch->vertices_index + 16] = normalized_color.y;
          batch->vertex_buffer[batch->vertices_index + 17] = normalized_color.z;

          batch->vertex_buffer[batch->vertices_index + 18] = final_position.x + final_position.w;
          batch->vertex_buffer[batch->vertices_index + 19] = final_position.y - final_position.h;
		  // batch->vertex_buffer[batch->vertices_index + 22] = final_layer;
          batch->vertex_buffer[batch->vertices_index + 20] = bottom_right_clip.x;
          batch->vertex_buffer[batch->vertices_index + 21] = bottom_right_clip.y;
          batch->vertex_buffer[batch->vertices_index + 22] = texture_slot_id;
          batch->vertex_buffer[batch->vertices_index + 23] = normalized_alpha_value;
          batch->vertex_buffer[batch->vertices_index + 24] = normalized_color.x;
          batch->vertex_buffer[batch->vertices_index + 25] = normalized_color.y;
          batch->vertex_buffer[batch->vertices_index + 26] = normalized_color.z;

          batch->vertex_buffer[batch->vertices_index + 27] = final_position.x + final_position.w;
          batch->vertex_buffer[batch->vertices_index + 28] = final_position.y;
		  // batch->vertex_buffer[batch->vertices_index + 32] = final_layer;
          batch->vertex_buffer[batch->vertices_index + 29] = top_right_clip.x;
          batch->vertex_buffer[batch->vertices_index + 30] = top_right_clip.y;
          batch->vertex_buffer[batch->vertices_index + 31] = texture_slot_id;
          batch->vertex_buffer[batch->vertices_index + 32] = normalized_alpha_value;
          batch->vertex_buffer[batch->vertices_index + 33] = normalized_color.x;
          batch->vertex_buffer[batch->vertices_index + 34] = normalized_color.y;
          batch->vertex_buffer[batch->vertices_index + 35] = normalized_color.z;

          batch->vertices_index += RendererInfo::FLOATS_PER_QUAD;
          batch->number_of_quads_to_copy++;
          batch->total_indices_to_draw += RendererInfo::INDICES_PER_QUAD;

          assert(batch->number_of_quads_to_copy <= RendererInfo::QUADS_PER_BATCH);


     }

}

void render_quad(Renderer *renderer, Rect *position, Texture *texture, Rect *clip_region, bool mirrorX, float alpha_value, V3 color , bool mirrorY){
     //This is temporary and it should actually select the next available batch when the current one gets filled or the
     //max amount of textures gets bound.
     if(renderer->current_shader.id != renderer->default_shader_program.id) {
          // renderer->current_shader = renderer->default_shader_program;
          // renderer->current_batch->shader_program = renderer->current_shader;
		  
		   renderer->batch_index++;
          renderer->current_batch = &renderer->batches[renderer->batch_index];
          renderer->current_shader = renderer->default_shader_program;
          renderer->current_batch->shader_program = renderer->current_shader;
     }
     else if(renderer->current_batch->number_of_quads_to_copy == RendererInfo::QUADS_PER_BATCH || renderer->current_batch->texture_index == RendererInfo::MAX_TEXTURE_UNITS_PER_BATCH){

          // renderer->current_batch->texture_index = 0;
          renderer->batch_index++;
          renderer->current_batch = &renderer->batches[renderer->batch_index];
          renderer->current_shader = renderer->default_shader_program;
          renderer->current_batch->shader_program = renderer->current_shader;
          // if(renderer->batch_index > 4) renderer->batch_index = 0;
     }
     // printf("Batch: %d,  Texture Index: %d\n", renderer->batch_index, renderer->current_batch->texture_index);
      assert(renderer->batch_index < RendererInfo::NUMBER_OF_BATCHES);
     Batch *batch = renderer->current_batch;

     render_quad_on_batch(renderer, batch, position, texture, clip_region, mirrorX, alpha_value, color, mirrorY);
}

void render_colored_rect(Renderer *renderer, Rect *position, V3 color, float alpha_value, ShaderProgram *shader){
	if(!shader){
		render_quad(renderer, position, NULL, NULL, false, alpha_value, color);
	}
	else{
		render_quad_with_shader(renderer, position, NULL, *shader, NULL, false, alpha_value, color);
	}
}

void render_quad_with_shader(Renderer *renderer, Rect *position, Texture *texture,ShaderProgram shader , Rect *clip_region, bool mirrorX, float alpha_value, V3 color , bool mirrorY){
     if(renderer->current_batch->number_of_quads_to_copy == RendererInfo::QUADS_PER_BATCH || renderer->current_batch->texture_index == RendererInfo::MAX_TEXTURE_UNITS_PER_BATCH || renderer->current_shader.id != shader.id){

          // renderer->current_batch->texture_index = 0;
          renderer->batch_index++;
          renderer->current_batch = &renderer->batches[renderer->batch_index];
          renderer->current_shader = shader;
          renderer->current_batch->shader_program = shader;
          // if(renderer->batch_index > 4) renderer->batch_index = 0;
     }
     // printf("Batch: %d,  Texture Index: %d\n", renderer->batch_index, renderer->current_batch->texture_index);
     assert(renderer->batch_index < RendererInfo::NUMBER_OF_BATCHES);
     Batch *batch = renderer->current_batch;

     render_quad_on_batch(renderer, batch, position, texture, clip_region, mirrorX, alpha_value, color, mirrorY);

}

static void clear_batches(Renderer *renderer){
     renderer->current_batch = &renderer->batches[0];
     renderer->batch_index = 0;
     //We clear the batches from any registered textures and reset the texture index so that in the next
     //frame we can repopulate the batches with new textures.
     for(int i = 0; i < RendererInfo::NUMBER_OF_BATCHES; ++i){
          renderer->batches[i].texture_index = 0;
		  renderer->batches[i].vertices_index = 0;
          renderer->batches[i].number_of_quads_to_copy = 0;
          renderer->batches[i].total_indices_to_draw = 0;
          for(int j = 0; j < RendererInfo::MAX_TEXTURE_UNITS_PER_BATCH; ++j){
               renderer->batches[i].registered_textures_ids[j] = 0;
          }
     }
}

void print_batching_info(Renderer *renderer){
     printf("Batches in use: %d\n", renderer->batch_index + 1);
     for(int i = 0; i <= renderer->batch_index; ++i){
          printf("Batch #%d\n", i);
          printf("Quads: %d\n", renderer->batches[i].number_of_quads_to_copy);
          printf("Textures: %d\n", renderer->batches[i].texture_index);
          printf("Shader: %s\n\n", renderer->batches[i].shader_program.name);
     }
}

void renderer_draw(Renderer *renderer){
     // print_batching_info(renderer);


     //We need to rebind the registered textures in each batch before drawing to the framebuffer.
     //If we don't the batch's textures get overwritten by the framebuffer texture. And we don't want
     //to reserve a texture unit just for the framebuffer.
     //What happened was that we used the texture unit 0 for the frambuffer texture, so the first texture
     //of the batch would get overwritten.
	 // print_batching_info(renderer);
	glBindFramebuffer(GL_FRAMEBUFFER, renderer->fbo);
	glEnable(GL_BLEND);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// This makes sure that the batches are scaled properly when changing the size of the window.
	glViewport(0,0,(int)renderer->drawing_resolution.x, (int)renderer->drawing_resolution.y); 
     for(int i = 0; i < RendererInfo::NUMBER_OF_BATCHES; ++i){
		glUseProgram(renderer->batches[i].shader_program.id);

		rebind_registered_texture_ids(&renderer->batches[i]);

		glBindVertexArray(renderer->batches[i].vao);
		glBindBuffer(GL_ARRAY_BUFFER, renderer->batches[i].vbo);

		int bytes_to_copy = renderer->batches[i].vertices_index * sizeof(float);
		//We only copy the vertex data that we are going to draw instead of copying the entire preallocated buffer.
		//This way we can preallocate a vertex buffer of any size and not affect performance.
		glBufferSubData(GL_ARRAY_BUFFER, 0, bytes_to_copy, (void*)renderer->batches[i].vertex_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->ibo);
		glDrawElements(GL_TRIANGLES, renderer->batches[i].total_indices_to_draw, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// renderer->batches[i].vertices_index = 0;
		// renderer->batches[i].number_of_quads_to_copy = 0;
		// renderer->batches[i].total_indices_to_draw = 0;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	int width, height;
	glfwGetWindowSize(renderer->window->GLFWInstance, &width, &height);
	glViewport(0, 0, width, height);
	draw_framebuffer(renderer);


	// print_batching_info(renderer);
	clear_batches(renderer);

}

void create_shader_program(ShaderProgram *program, unsigned int vertex_shader, unsigned int fragment_shader){
     program->id = glCreateProgram();
     unsigned int id = program->id;
     glAttachShader(id, vertex_shader);
     glAttachShader(id, fragment_shader);
     glLinkProgram(id);
     int success;
     glGetProgramiv(id, GL_LINK_STATUS, &success);
     if( success == GL_FALSE){
          printf("Could not link the shader program \n");
          exit(1);
     }

     glDetachShader(id, vertex_shader);
     glDetachShader(id, fragment_shader);
     glDeleteShader(vertex_shader);
     glDeleteShader(fragment_shader);
}

static void make_vertex_shader(const char* path, unsigned int *vertex_shader){
     char *source = text_file_to_char(path);
     *vertex_shader = glCreateShader(GL_VERTEX_SHADER);
     glShaderSource(*vertex_shader, 1 , &source, NULL);
     glCompileShader(*vertex_shader);

     int  success;
     char infoLog[512];
     glGetShaderiv(*vertex_shader, GL_COMPILE_STATUS, &success);

     if(!success)
     {
         glGetShaderInfoLog(*vertex_shader, 512, NULL, infoLog);
         printf("Vertex shader compilation failed: \n %s", infoLog);
     }
     else{
          // printf("Vertex shader succesfully compiled \n");
     }
     free(source);
}

static void make_vertex_shader_from_source(const char* shader, unsigned int *vertex_shader){
     *vertex_shader = glCreateShader(GL_VERTEX_SHADER);
     glShaderSource(*vertex_shader, 1 , &shader, NULL);
     glCompileShader(*vertex_shader);

     int  success;
     char infoLog[512];
     glGetShaderiv(*vertex_shader, GL_COMPILE_STATUS, &success);

     if(!success)
     {
         glGetShaderInfoLog(*vertex_shader, 512, NULL, infoLog);
         printf("Vertex shader compilation failed: \n %s", infoLog);
     }
     else{
          // printf("Vertex shader succesfully compiled \n");
     }
}

static void make_fragment_shader(const char* path, unsigned int *fragment_shader){
     char *source = text_file_to_char(path);
     *fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
     glShaderSource(*fragment_shader, 1 , &source, NULL);
     glCompileShader(*fragment_shader);

     int  success;
     char infoLog[512];
     glGetShaderiv(*fragment_shader, GL_COMPILE_STATUS, &success);

     if(!success)
     {
         glGetShaderInfoLog(*fragment_shader, 512, NULL, infoLog);
         printf("Fragmetn shader compilation failed: \n %s", infoLog);
     }
     else{
          // printf("Fragment shader succesfully compiled \n");
     }
     free(source);
}

static void make_fragment_shader_from_source(const char* shader, unsigned int *fragment_shader){
     *fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
     glShaderSource(*fragment_shader, 1 , &shader, NULL);
     glCompileShader(*fragment_shader);

     int  success;
     char infoLog[512];
     glGetShaderiv(*fragment_shader, GL_COMPILE_STATUS, &success);

     if(!success)
     {
         glGetShaderInfoLog(*fragment_shader, 512, NULL, infoLog);
         printf("Fragmetn shader compilation failed: \n %s", infoLog);
     }
     else{
          // printf("Fragment shader succesfully compiled \n");
     }
}

ShaderProgram make_shader(Renderer *renderer, const char *path_to_fragment_shader, const char *name){
	ShaderProgram shader;
	shader.name = name;
	unsigned int vs;
	unsigned int fs;
	make_vertex_shader_from_source(default_vertex_shader, &vs);
	make_fragment_shader(path_to_fragment_shader, &fs);
	shader.id = glCreateProgram();
	glAttachShader(shader.id, vs);
	glAttachShader(shader.id, fs);
	glLinkProgram(shader.id);
	int success;
	glGetProgramiv(shader.id, GL_LINK_STATUS, &success);
	if( success == GL_FALSE){
	  printf("Could not link the shader program \n");
	  exit(1);
	}

	glDetachShader(shader.id, vs);
	glDetachShader(shader.id, fs);
	glDeleteShader(vs);
	glDeleteShader(fs);

	printf("SHADER PROGRAM %s\n", shader.name);
	printf("Succesfully compiled\n\n");
	initialize_texture_sampler(shader);
	load_mvp_to_shader(renderer, shader);
	return shader;
}




static void bind_texture(int slot, unsigned int id){
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, id);
}

static bool check_if_texture_is_not_registered(Texture texture, Batch *batch){
     int difference_count = 0;
     // static int count = 0;
     for(int i = 0; i < RendererInfo::MAX_TEXTURE_UNITS_PER_BATCH; i++){
          if(texture.id != batch->registered_textures_ids[i]){
               difference_count++;
          }
     }
     if(difference_count == RendererInfo::MAX_TEXTURE_UNITS_PER_BATCH){
          // count++;
          // printf("%d ", count );
          //
          // if(count >= 2){
          //      count = 0;
          //      printf("\n");
          // }
          return true;
     }else{
          // printf("%d ", count );
          return false;
     }
}

static void rebind_registered_texture_ids(Batch *batch){
     for(int i = 0; i < RendererInfo::MAX_TEXTURE_UNITS_PER_BATCH; i++){
          bind_texture( i, batch->registered_textures_ids[i]);
     }
}

void print_rect(Rect *rect){
	printf("x: %f, y: %f, w: %f, h: %f\n", rect->x, rect->y, rect->w, rect->h);
}
