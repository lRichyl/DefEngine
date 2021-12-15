#include "glad/glad.h"
#include "utilities.h"
#include "renderer.h"
#include "math.h"
#include "stb_image.h"
#include <assert.h>

bool check_if_texture_is_not_registered(Texture texture, Batch *batch);
void bind_texture(int slot, unsigned int id);
static void rebind_registered_texture_ids(Batch *batch);
void create_shader_program(ShaderProgram *program, unsigned int vertex_shader, unsigned int fragment_shader);
void make_vertex_shader(const char* path, unsigned int *vertex_shader);
void make_fragment_shader(const char* path, unsigned int *fragment_shader);

int RendererInfo::MAX_TEXTURE_UNITS_PER_BATCH = 0;

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
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
     glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
     glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
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
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
     glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
     glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
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
     glVertexAttribPointer(0, 3, GL_FLOAT, false, 10 * sizeof(float), (void*)0);
     glEnableVertexAttribArray(1);
     glVertexAttribPointer(1, 2, GL_FLOAT, false, 10 * sizeof(float), (void*)( 3 * sizeof(float)));
     glEnableVertexAttribArray(2);
     glVertexAttribPointer(2, 1, GL_FLOAT, false, 10 * sizeof(float), (void*)( 5 * sizeof(float)));
     glEnableVertexAttribArray(3);
     glVertexAttribPointer(3, 1, GL_FLOAT, false, 10 * sizeof(float), (void*)( 6 * sizeof(float)));
     glEnableVertexAttribArray(4);
     glVertexAttribPointer(4, 3, GL_FLOAT, false, 10 * sizeof(float), (void*)( 7 * sizeof(float)));


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

void initialize_renderer(Renderer *renderer, Window *window){
     renderer->projection = glm::ortho(0.0f, (float)window->internalWidth, 0.0f, (float)window->internalHeight, 0.0f, -100.f);
     renderer->view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)); //Modify this in real time to move the camera.
     renderer->drawing_resolution.x = window->internalWidth;
     renderer->drawing_resolution.y = window->internalHeight;
     renderer->current_batch = &renderer->batches[0];

     renderer->default_shader_program.name = "Batch shader";
     renderer->framebuffer_shader_program.name = "Framebuffer shader";
     compile_shader_program(&renderer->default_shader_program, "assets/shaders/default_vertex_shader.txt", "assets/shaders/default_fragment_shader.txt");
     compile_shader_program(&renderer->framebuffer_shader_program, "assets/shaders/framebuffer_vertex_shader.txt", "assets/shaders/framebuffer_fragment_shader.txt");

     create_framebuffer_buffers(renderer);
     initialize_framebuffer(renderer);

     initialize_renderer_index_buffer(renderer);
     //This should be in a loop when we add multiple batches to initialize them.
     for(int i = 0; i < RendererInfo::NUMBER_OF_BATCHES; ++i){
          initialize_batch_vertex_buffers_and_arrays(&renderer->batches[i], renderer);
     }
     initialize_texture_sampler(renderer->default_shader_program);
     //This needs to happen after the shader program is compiled.

     initialize_batch_vertex_buffers_and_arrays(&renderer->ui_batch, renderer);
     initialize_texture_sampler(renderer->ui_batch.shader_program);
     load_mvp_to_shader(renderer, renderer->default_shader_program);

}

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

static void render_quad_on_batch(Renderer *renderer, Batch *batch, Rect *position, Texture *texture, Rect *clip_region, int layer,  bool mirrorX, float alpha_value, V3 color, bool mirrorY){
     Window *win = renderer->window;
     V2 top_left_clip;
     V2 bottom_left_clip;
     V2 top_right_clip;
     V2 bottom_right_clip;
     float normalizedAlphaValue = alpha_value; // / 255.f;
     Rect final_position;
	 // float layer  = 0;
	 int max_layers = 100;
	 float final_layer = (float)layer;
	 
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
          // Batch *current_batch = &renderer->main_batch;
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
		  batch->vertex_buffer[batch->vertices_index + 2] = final_layer;
          batch->vertex_buffer[batch->vertices_index + 3] = top_left_clip.x;
          batch->vertex_buffer[batch->vertices_index + 4] = top_left_clip.y;
          batch->vertex_buffer[batch->vertices_index + 5] = texture_slot_id;
          batch->vertex_buffer[batch->vertices_index + 6] = normalizedAlphaValue;
          batch->vertex_buffer[batch->vertices_index + 7] = color.x;
          batch->vertex_buffer[batch->vertices_index + 8] = color.y;
          batch->vertex_buffer[batch->vertices_index + 9] = color.z;

          batch->vertex_buffer[batch->vertices_index + 10] = final_position.x;
          batch->vertex_buffer[batch->vertices_index + 11] = final_position.y - final_position.h;
		  batch->vertex_buffer[batch->vertices_index + 12] = final_layer;
          batch->vertex_buffer[batch->vertices_index + 13] = bottom_left_clip.x;
          batch->vertex_buffer[batch->vertices_index + 14] = bottom_left_clip.y;
          batch->vertex_buffer[batch->vertices_index + 15] = texture_slot_id;
          batch->vertex_buffer[batch->vertices_index + 16] = normalizedAlphaValue;
          batch->vertex_buffer[batch->vertices_index + 17] = color.x;
          batch->vertex_buffer[batch->vertices_index + 18] = color.y;
          batch->vertex_buffer[batch->vertices_index + 19] = color.z;

          batch->vertex_buffer[batch->vertices_index + 20] = final_position.x + final_position.w;
          batch->vertex_buffer[batch->vertices_index + 21] = final_position.y - final_position.h;
		  batch->vertex_buffer[batch->vertices_index + 22] = final_layer;
          batch->vertex_buffer[batch->vertices_index + 23] = bottom_right_clip.x;
          batch->vertex_buffer[batch->vertices_index + 24] = bottom_right_clip.y;
          batch->vertex_buffer[batch->vertices_index + 25] = texture_slot_id;
          batch->vertex_buffer[batch->vertices_index + 26] = normalizedAlphaValue;
          batch->vertex_buffer[batch->vertices_index + 27] = color.x;
          batch->vertex_buffer[batch->vertices_index + 28] = color.y;
          batch->vertex_buffer[batch->vertices_index + 29] = color.z;

          batch->vertex_buffer[batch->vertices_index + 30] = final_position.x + final_position.w;
          batch->vertex_buffer[batch->vertices_index + 31] = final_position.y;
		  batch->vertex_buffer[batch->vertices_index + 32] = final_layer;
          batch->vertex_buffer[batch->vertices_index + 33] = top_right_clip.x;
          batch->vertex_buffer[batch->vertices_index + 34] = top_right_clip.y;
          batch->vertex_buffer[batch->vertices_index + 35] = texture_slot_id;
          batch->vertex_buffer[batch->vertices_index + 36] = normalizedAlphaValue;
          batch->vertex_buffer[batch->vertices_index + 37] = color.x;
          batch->vertex_buffer[batch->vertices_index + 38] = color.y;
          batch->vertex_buffer[batch->vertices_index + 39] = color.z;

          batch->vertices_index += RendererInfo::FLOATS_PER_QUAD;
          batch->number_of_quads_to_copy++;
          batch->total_indices_to_draw += RendererInfo::INDICES_PER_QUAD;

          assert(batch->number_of_quads_to_copy <= RendererInfo::QUADS_PER_BATCH);


     }

}

void render_quad(Renderer *renderer, Rect *position, Texture *texture, int layer, Rect *clip_region, bool mirrorX, float alpha_value, V3 color , bool mirrorY){
     //This is temporary and it should actually select the next available batch when the current one gets filled or the
     //max amount of textures gets bound.
     if(renderer->current_shader.id != renderer->default_shader_program.id) {
          renderer->current_shader = renderer->default_shader_program;
          renderer->current_batch->shader_program = renderer->current_shader;
     }
     if(renderer->current_batch->number_of_quads_to_copy == RendererInfo::QUADS_PER_BATCH || renderer->current_batch->texture_index == RendererInfo::MAX_TEXTURE_UNITS_PER_BATCH){

          // renderer->current_batch->texture_index = 0;
          renderer->batch_index++;
          renderer->current_batch = &renderer->batches[renderer->batch_index];
          // renderer->current_shader = renderer->default_shader_program;
          // renderer->current_batch->shader_program = renderer->current_shader;
          // if(renderer->batch_index > 4) renderer->batch_index = 0;
     }
     // printf("Batch: %d,  Texture Index: %d\n", renderer->batch_index, renderer->current_batch->texture_index);
     assert(renderer->batch_index < 5);
     Batch *batch = renderer->current_batch;

     render_quad_on_batch(renderer, batch, position, texture, clip_region, layer, mirrorX, alpha_value, color, mirrorY);
}

void render_colored_rect(Renderer *renderer, Rect *position, V3 color, float alpha_value, int layer){
	render_quad(renderer, position, NULL, layer, NULL, false, alpha_value, color);
}

//Things drawn with this function do no get drawn on the same framebuffer as the main render_quad function so it does not get
//the postprocessing effects. Mainly used to do UI stuff.
void render_quad_to_ui(Renderer *renderer, Rect *position, Texture *texture, Rect *clip_region, int layer, bool mirrorX, float alpha_value, V3 color, bool mirrorY){
     render_quad_on_batch(renderer, &renderer->ui_batch, position, texture, clip_region, layer, mirrorX, alpha_value, color, mirrorY);
}

void render_quad_with_shader(Renderer *renderer, Rect *position, Texture *texture,ShaderProgram shader ,Rect *clip_region, int layer, bool mirrorX, float alpha_value, V3 color , bool mirrorY){
     if(renderer->current_batch->number_of_quads_to_copy == RendererInfo::QUADS_PER_BATCH || renderer->current_batch->texture_index == RendererInfo::MAX_TEXTURE_UNITS_PER_BATCH || renderer->current_shader.id != shader.id){

          // renderer->current_batch->texture_index = 0;
          renderer->batch_index++;
          renderer->current_batch = &renderer->batches[renderer->batch_index];
          renderer->current_shader = shader;
          renderer->current_batch->shader_program = shader;
          // if(renderer->batch_index > 4) renderer->batch_index = 0;
     }
     // printf("Batch: %d,  Texture Index: %d\n", renderer->batch_index, renderer->current_batch->texture_index);
     assert(renderer->batch_index < 5);
     Batch *batch = renderer->current_batch;

     render_quad_on_batch(renderer, batch, position, texture, clip_region, layer, mirrorX, alpha_value, color, mirrorY);

}

static void clear_batches(Renderer *renderer){
     renderer->current_batch = &renderer->batches[0];
     renderer->batch_index = 0;
     //We clear the batches from any registered textures and reset the texture index so that in the next
     //frame we can repopulate the batches with new textures.
     for(int i = 0; i < RendererInfo::NUMBER_OF_BATCHES; ++i){
          renderer->batches[i].texture_index = 0;
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
     // Batch *current_batch = &renderer->main_batch;


     //We need to rebind the registered textures in each batch before drawing to the framebuffer.
     //If we don't the batch's textures get overwritten by the framebuffer texture. And we don't want
     //to reserve a texture unit just for the framebuffer.
     //What happened was that we used the texture unit 0 for the frambuffer texture, so the first texture
     //of the batch would get overwritten.
	 // print_batching_info(renderer);
     glBindFramebuffer(GL_FRAMEBUFFER, renderer->fbo);
     glEnable(GL_BLEND);
     // glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
     // glEnable(GL_DEPTH_TEST);
     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
     for(int i = 0; i < RendererInfo::NUMBER_OF_BATCHES; ++i){
          glViewport(0,0,(int)renderer->drawing_resolution.x, (int)renderer->drawing_resolution.y);
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

          renderer->batches[i].vertices_index = 0;
          renderer->batches[i].number_of_quads_to_copy = 0;
          renderer->batches[i].total_indices_to_draw = 0;
          // renderer->batches[i].texture_index = 0;

     }
	 // glClear(GL_DEPTH_BUFFER_BIT);
     glBindFramebuffer(GL_FRAMEBUFFER, 0);


     // glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
     // glDisable(GL_DEPTH_TEST);

     // renderer->batches[i].texture_index = 0;
     int width, height;
     glfwGetWindowSize(renderer->window->GLFWInstance, &width, &height);

     glViewport(0, 0, width, height);
     draw_framebuffer(renderer);


//----------------UI will be rendered after the framebuffer so that shaders related to the aspect of the game do not affect it---------------------------------------
     {
     rebind_registered_texture_ids(&renderer->ui_batch);

          glUseProgram(renderer->ui_batch.shader_program.id);
          glBindVertexArray(renderer->ui_batch.vao);
          glBindBuffer(GL_ARRAY_BUFFER, renderer->ui_batch.vbo);

          int bytes_to_copy = renderer->ui_batch.vertices_index * sizeof(float);
          //We only copy the vertex data that we are going to draw instead of copying the entire preallocated buffer.
          //This way we can preallocate a vertex buffer of any size and not affect performance.
          glBufferSubData(GL_ARRAY_BUFFER, 0, bytes_to_copy, (void*)renderer->ui_batch.vertex_buffer);
          glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->ibo);
          glDrawElements(GL_TRIANGLES, renderer->ui_batch.total_indices_to_draw, GL_UNSIGNED_INT, 0);
          glBindVertexArray(0);

          renderer->ui_batch.vertices_index = 0;
          renderer->ui_batch.number_of_quads_to_copy = 0;
          renderer->ui_batch.total_indices_to_draw = 0;
     }
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

void make_vertex_shader(const char* path, unsigned int *vertex_shader){
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

void make_fragment_shader(const char* path, unsigned int *fragment_shader){
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

// TODO: Add the ability to choose the type of filter when creating the texture.
Texture make_texture(const char *path){
     Texture texture;
     texture.data_buffer = stbi_load(path, &texture.width, &texture.height, &texture.channels, 0);
     if(!texture.data_buffer){
          printf("Could not find a texture image at the relative path: %s\n", path);
          exit(-1);
     }
	 
	 if(texture.channels == 4){
		glGenTextures(1, &texture.id);
		// printf("Texture ID: %d\n", texture.id);
		glBindTexture(GL_TEXTURE_2D, texture.id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width, texture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)texture.data_buffer);
		glBindTexture(GL_TEXTURE_2D, 0);
		stbi_image_free(texture.data_buffer);
	 }else if(texture.channels == 3){
		glGenTextures(1, &texture.id);
		// printf("Texture ID: %d\n", texture.id);
		glBindTexture(GL_TEXTURE_2D, texture.id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture.width, texture.height, 0, GL_RGB, GL_UNSIGNED_BYTE, (void*)texture.data_buffer);
		glBindTexture(GL_TEXTURE_2D, 0);
		stbi_image_free(texture.data_buffer);
	 }
     return texture;
}

void bind_texture(int slot, unsigned int id){
     glActiveTexture(GL_TEXTURE0 + slot);
     glBindTexture(GL_TEXTURE_2D, id);
}

bool check_if_texture_is_not_registered(Texture texture, Batch *batch){
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
