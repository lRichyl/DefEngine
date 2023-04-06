#ifndef DEF_MATH_H
#define DEF_MATH_H
#define F_ZERO_APPROXIMATION 0.00000001f
#define F_INVERSE_SQRT_2 0.70710678118f

namespace def {
     typedef struct def_f_vec_2{
          float x;
          float y;
          def_f_vec_2() :x(0), y(0) {}
          def_f_vec_2(float x, float y) : x(x), y(y) {}
          static void SwitchXComponents(def_f_vec_2 *a, def_f_vec_2 *b);
          static void SwitchYComponents(def_f_vec_2 *a, def_f_vec_2 *b);
          static def_f_vec_2 Normalized(const def_f_vec_2 &val);
          static float Magnitude(const def_f_vec_2 &val);
          //todo:: operator overload... check vectorian library...
     }VEC_2D;

     typedef struct def_f_vec_3{
          float x;
          float y;
          float z;
          def_f_vec_3() :x(0), y(0), z(0) {}
          def_f_vec_3(float x, float y, float z) : x(x), y(y), z(z) {}
     }VEC_3D;

     typedef struct def_f_vec_4{
          float x = 0;
          float y = 0;
          float z = 0;
     	 float w = 0;
          def_f_vec_4() :x(0), y(0), z(0), w(0) {}
          def_f_vec_4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
     }VEC_4D;

     typedef struct def_f_rect{
     	float x;
     	float y;
     	float w;
     	float h;
         def_f_rect() :x(0), y(0), w(0), h(0) {}
         def_f_rect(float x, float y, float w, float h) : x(x), y(y), w(w), h(h) {}
     }RECT;

     float abs(float val) noexcept;
     float sign(float val) noexcept;
}

#endif