#ifndef DEF_MATH_H
#define DEF_MATH_H

namespace def {
     struct V2{
          float x;
          float y;
          V2() :x(0), y(0) {}
          V2(float x, float y) : x(x), y(y) {}
          static void SwitchXComponents(V2 *a, V2 *b);
          static void SwitchYComponents(V2 *a, V2 *b);
     };

     struct V3{
          float x;
          float y;
          float z;
          V3() :x(0), y(0), z(0) {}
          V3(float x, float y, float z) : x(x), y(y), z(z) {}
     };

     struct V4{
          float x = 0;
          float y = 0;
          float z = 0;
     	 float w = 0;
          V4() :x(0), y(0), z(0), w(0) {}
          V4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
     };

     struct Rect{
     	float x;
     	float y;
     	float w;
     	float h;
         Rect() :x(0), y(0), w(0), h(0) {}
         Rect(float x, float y, float w, float h) : x(x), y(y), w(w), h(h) {}
     };
}

#endif