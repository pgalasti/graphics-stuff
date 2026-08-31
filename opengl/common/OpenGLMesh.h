#ifndef GSTUFF_OPENGL_MESH_H
#define GSTUFF_OPENGL_MESH_H

#include "general/Mesh.h"
#include "general/Math.h"

#include "defines.h"

#include "Texture.h"

namespace GStuff::OpenGL {

template <typename VertexType, typename TextureID>
using BaseMesh = GStuff::General::Mesh<VertexType, TextureID>;

using namespace GStuff::General::Math;

class OpenGLMesh : public BaseMesh<Vertex3DNUVf, GLuint> {
public:
  
  OpenGLMesh(const std::vector<Vertex3DNUVf>& vertices, const std::vector<unsigned int>& indices)
    : BaseMesh<Vertex3DNUVf, GLuint>(vertices, indices) { Init(); }
  ~OpenGLMesh() { Teardown(); }

  void Draw(Program* pProgram) override;
  void Setup() override;
  void SetTexture(GStuff::General::BaseTexture<GLuint>* pTexture) override; 

private:
  ObjID m_VAO;
  ObjID m_VBO;
  ObjID m_EBO;

  void Init();
  void Teardown();

};

}

#endif
