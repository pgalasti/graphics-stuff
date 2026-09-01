#ifndef GSTUFF_MESH_H
#define GSTUFF_MESH_H

#include "Shaders.h"
#include "BaseTexture.h"

#include <vector>

namespace GStuff::General {

template <typename VertexType, typename TextureID>
class Mesh {
public:

  Mesh(const std::vector<VertexType>& vertices, const std::vector<unsigned int>& indices) 
    : m_Vertices{vertices}, m_Indices{indices} {}
  virtual ~Mesh() = default;

  using Program = GStuff::General::Shaders::Program;
  using TexturePtrList = std::vector<std::pair<BaseTexture<TextureID>*, TextureID>>;

  virtual void Draw(Program* pProgram) = 0;
  virtual void Setup() = 0;
  virtual void SetTexture(BaseTexture<TextureID>*, TextureID positionNum) = 0;

protected:

  std::vector<VertexType> m_Vertices;
  std::vector<unsigned int> m_Indices;
  TexturePtrList m_TexturePtrs; 
};

}

#endif
