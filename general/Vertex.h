#ifndef G_STUFF_VERTEX_H
#define G_STUFF_VERTEX_H

namespace GStuff::General {

template <typename T>
struct Vertex3D {
  union {
    T vals[3] {};
    struct {
      T x, y, z;
    };
  };
};

template <typename T>
struct Vertex3DUV {
  union {
    T vals[5] {};
    struct {
      T x, y, z, u, v;
    };
  };
};

template <typename T>
struct Vertex3DRGB {
  union {
    T vals[6] {};
    struct {
      T x, y, z, r, g, b;
    };
  };
};

template <typename T>
struct Vertex3DRGBUV {
  union {
    T vals[8] {};
    struct {
      T x, y, z, r, g, b, u, v;
    };
  };
};

}
#endif
