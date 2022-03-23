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

#define DEV

enum GameState{
	GAME_PLAY,
	GAME_EDITOR
};

struct Game{
	void UpdateGame(float dt);
	void DrawGame(float dt, float fps);
	void GameLoop(float dt, float fps);
	Game(Renderer *r, Window *w);
	
	Renderer *renderer;
	Window *window;
	bool showFPS = true;
	bool showEditor = false;
	GameState state = GameState::GAME_PLAY;
	
	static MouseInfo     mouse;
	static AssetManager  asset_manager;
	static MemoryArena   main_arena;
	static EntityManager em;
	static float         dt;
	static Camera        camera;
	
	Font font  = Font("assets/fonts/Simvoni.ttf", 16);
	// Frame frame;

	LevelEditor level_editor;
	Texture texture;
	Texture smiley;
	Texture link;
	Texture frame_texture;
};
