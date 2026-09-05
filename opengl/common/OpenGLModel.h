#ifndef GSTUFF_OPENGL_MODEL_H
#define GSTUFF_OPENGL_MODEL_H

#include "general/Math.h"
#include "general/Model.h"
#include "general/Shaders.h"

#include "OpenGLMesh.h"
#include "Texture.h"
#include "AssimpModelLoader.h"

#include <memory>
#include <string>
#include <unordered_map>

namespace GStuff::OpenGL {

using namespace GStuff::General;
using namespace GStuff::General::Math;

class Vertex3DNUVfModel : public Model<OpenGLMesh3DNUVf> {
  using Base = GStuff::General::Model<OpenGLMesh3DNUVf>;
public:

  static constexpr GLuint DIFFUSE_UNIT  {0};
  static constexpr GLuint SPECULAR_UNIT {1};

  Vertex3DNUVfModel(const std::string& modelPath);
  ~Vertex3DNUVfModel() = default;

  void Draw(Program* pProgram) override;

  // Need to make this a part of base
  void SetTexture(GStuff::General::BaseTexture<GLuint>* pTexture, GLuint unit);

private:

  std::unordered_map<std::string, std::unique_ptr<Texture>> m_TextureCache;

  Texture* LoadTexture(const std::string& texturePath, GLuint unit, const std::string& samplerName);

};


}

#endif
