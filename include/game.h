#pragma once

#include "time.h"
#include "renderer.h"
#include "text.h"
#include "asset_manager.h"
#include "input.h"
#include "audio.h"
#include "gui.h"
#include "level_editor.h"
#include "camera.h"
#include "console.h"

#define DEV 1

const int UNICODE_BUFFER_SIZE = 10;

enum GameState{
	GAME_PLAY,
	GAME_EDITOR
};

struct Game{
	void UpdateGame(float dt);
	void DrawGame(float dt, float fps);
	void GameLoop(float dt, float fps);
	Game(Renderer *r, Window *w);

	static void clear_render_lists();
	
	Renderer *renderer;
	Window *window;
	bool showFPS = true;
	GameState state = GameState::GAME_PLAY;
	
	static MouseInfo              mouse;
	static AssetManager           asset_manager;
	static MemoryArena            main_arena;
	static EntityManager          em;
	static float                  dt;
	static Console                console;
	static Camera                 camera;
	static DefArray<RenderCommand> layers_render_commands[LEVEL_LAYERS];

	// Font font  = Font("assets/fonts/Simvoni.ttf", 16);
	// Frame frame;

	LevelEditor level_editor;
};
