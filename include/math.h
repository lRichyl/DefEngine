#pragma once

struct V2{
     float x = 0;
     float y = 0;

     static void SwitchXComponents(V2 *a, V2 *b);
     static void SwitchYComponents(V2 *a, V2 *b);
};

struct V3{
     float x = 0;
     float y = 0;
     float z = 0;
};

struct V4{
     float x = 0;
     float y = 0;
     float z = 0;
	 float w = 0;
};

struct Rect{
	float x;
	float y;
	float w;
	float h;
};