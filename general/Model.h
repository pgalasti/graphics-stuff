#ifndef GSTUFF_MODEL_H
#define GSTUFF_MODEL_H

#include "Shaders.h"

#include <string>
#include <vector>

namespace GStuff::General {

template <typename MeshType>
class Model {
public:
  Model(const std::string& modelPath) 
    : m_FilePath{modelPath} {}
  virtual ~Model() = default;

  using Program = GStuff::General::Shaders::Program;
  virtual void Draw(Program*) = 0;
  
protected:
  std::string m_FilePath;
  std::vector<MeshType> m_Meshes;
};

}

#endif
