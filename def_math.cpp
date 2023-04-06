#include "def_math.h"
#include "math.h"
namespace def {
     // Mirror x.
     void VEC_2D::SwitchXComponents(VEC_2D *a, VEC_2D *b){
          float temporalX = a->x;

          a->x = b->x;
          b->x = temporalX;
     }

     // Mirror y.
     void VEC_2D::SwitchYComponents(VEC_2D *a, VEC_2D *b){
          float temporalY = a->y;

          a->y = b->y;
          b->y = temporalY;
     }


     float VEC_2D::Magnitude(const VEC_2D &val)
     {
    	 return sqrtf(val.x * val.x + val.y * val.y);
     }
     //ABS FUNC
     //Could be Calculated as abs(x) = sqrt(x * x)...
     //but is calculated as case statement:
     //|x| = {  x  if x ≥ 0  }
     //|x| = { -x  if x < 0  }
     //with special case when x is equal to zero, then return zero
     float abs(float val) noexcept
     {
    	 if(val < F_ZERO_APPROXIMATION && val > F_ZERO_APPROXIMATION)
			 return 0.0f;
    	 if(val >= F_ZERO_APPROXIMATION)
    		 return val;
    	 else
    		 return - val;
     }

     //SIGN FUN
     //Calculated as sign(x) = x / abs(x)
     //with special case when x is equal to zero, then return zero
     float sign(float val) noexcept
     {
    	 if(val < F_ZERO_APPROXIMATION && val > F_ZERO_APPROXIMATION)
    		 return 0.0f;
    	 return val / abs(val);
     }
     //divide each component by the vector magnitude
     VEC_2D VEC_2D::Normalized(const VEC_2D &val)
     {
    	 VEC_2D _out{0.0f, 0.0f};
    	 float magnitude = Magnitude(val);
    	 if(!abs(magnitude) > 0.0f)
    		 return _out;
    	 _out.x = val.x / magnitude;
    	 _out.y = val.y / magnitude;
    	 return _out;
     }
}
