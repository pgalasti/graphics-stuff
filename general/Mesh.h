#ifndef GSTUFF_MESH_H
#define GSTUFF_MESH_H

#include "Shaders.h"
#include "Texture.h"

#include <vector>

namespace GStuff::General {

template <typename VertexType>
class Mesh {
public:

  Mesh(const std::vector<VertexType>& vertices, const std::vector<unsigned int>& indices) 
    : m_Vertices{vertices}, m_Indices{indices} {}

  using Program = GStuff::General::Shaders::Program;

  virtual void Draw(Program* pProgram) = 0;
  virtual void Setup() = 0;
  virtual void SetTexture(BaseTexture*) = 0;

protected:

  std::vector<VertexType> m_Vertices;
  std::vector<unsigned int> m_Indices;
  BaseTexture* m_pTexture;
  
};

}

#endif
