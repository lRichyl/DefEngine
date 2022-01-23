#pragma once

#include "time.h"
#include "renderer.h"
#include "text.h"
#include "asset_manager.h"
#include "sprite.h"
#include "audio.h"


struct Game{
	void UpdateGame(float dt);
	void DrawGame(float dt, float fps);
	void GameLoop(float dt, float fps);
	Game(Renderer *r, Window *w);
	
	Renderer *renderer;
	Window *window;
	bool showFPS = true;
	
	static AssetManager asset_manager;
	
	Font font  =Font("assets/fonts/ChrustyRock.ttf", 40);
	Texture texture;
	EntityManager em;
};
