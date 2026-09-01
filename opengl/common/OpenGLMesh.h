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


// Each vertex typed mesh can extend off this guy and implement the appropriate
// vertex attribute description
template <typename TemplateType>
class OpenGLMesh : public GStuff::General::Mesh<TemplateType, GLuint> {
  using Base = GStuff::General::Mesh<TemplateType, GLuint>;
public:
  using typename Base::Program;
  using typename Base::TexturePtrList;
  using Base::m_Vertices;
  using Base::m_Indices;
  using Base::m_TexturePtrs;

  OpenGLMesh(const std::vector<TemplateType>& vertices, const std::vector<unsigned int>& indices)
    : Base(vertices, indices) {  }
  ~OpenGLMesh() { Teardown(); }

  void Draw(Program* pProgram) override {
    pProgram->Activate();
    glBindVertexArray(m_VAO);

    for(auto element : m_TexturePtrs) {
      auto[pTexture, positionNum] = element;

      auto constants {pTexture->GetConstants()};
      for(const auto& constantPair : constants) {
        const auto[pos, name] = constantPair;
        pProgram->SetConstant(name, static_cast<int>(pos));
      }
      pTexture->Bind(positionNum);
    }

    glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
  }
  void SetTexture(GStuff::General::BaseTexture<GLuint>* pTexture, GLuint positionNum) override {
    m_TexturePtrs.push_back({pTexture, positionNum});
  }
  
  virtual void Setup() override = 0;

protected:
  ObjID m_VAO;
  ObjID m_VBO;
  ObjID m_EBO;

  void Teardown() {
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
  }
};

class OpenGLMesh3DNUVf : public OpenGLMesh<Vertex3DNUVf> {
public:
  OpenGLMesh3DNUVf(const std::vector<Vertex3DNUVf>& vertices, const std::vector<unsigned int>& indices)
      : OpenGLMesh<Vertex3DNUVf>(vertices, indices) { Setup(); }
  ~OpenGLMesh3DNUVf() = default;

  void Setup() override;
};
class OpenGLMesh3Df : public OpenGLMesh<Vertex3Df> {
public:
  OpenGLMesh3Df(const std::vector<Vertex3Df>& vertices, const std::vector<unsigned int>& indices)
      : OpenGLMesh<Vertex3Df>(vertices, indices) { Setup(); }
  ~OpenGLMesh3Df() = default;

  void Setup() override;
};

}

#endif
