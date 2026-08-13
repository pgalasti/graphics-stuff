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

using Vertex3Df = Vertex3D<float>;

template <typename T>
struct Vertex3DUV {
  union {
    T vals[5] {};
    struct {
      T x, y, z, u, v;
    };
  };
};

using Vertex3DUVf = Vertex3DUV<float>;

template <typename T>
struct Vertex3DRGBUV {
  union {
    T vals[8] {};
    struct {
      T x, y, z, r, g, b, u, v;
    };
  };
};

using Vertex3DRGBUVf = Vertex3DRGBUV<float>;

}
#endif 
