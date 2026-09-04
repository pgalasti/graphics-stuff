#ifndef GSTUFF_OPENGL_MODEL_H
#define GSTUFF_OPENGL_MODEL_H

#include "general/Math.h"
#include "general/Model.h"
#include "general/Shaders.h"

#include "AssimpModelLoader.h"

namespace GStuff::OpenGL {

using namespace GStuff::General;
using namespace GStuff::General::Math;

class Vertex3DNUVfModel : public Model<Vertex3DNUVf> {
  using Base = GStuff::General::Model<Vertex3DNUVf>;
public:

  

  Vertex3DNUVfModel(const std::string& modelPath);
  ~Vertex3DNUVfModel() = default;

  void Draw(Program* pProgram) override;

};


}

#endif
