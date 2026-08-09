#ifndef GENERAL_COMMON_MATH_H
#define GENERAL_COMMON_MATH_H

namespace GStuff::General::Math {

template <typename T>
struct Vertex3D {
  union {
    T vals[3] {};
    struct {
      T x, y, z;
    };
  };
};

}
#endif 
