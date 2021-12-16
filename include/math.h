#ifndef MATH_H
#define MATH_H

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


#endif
