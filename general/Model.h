#ifndef GSTUFF_MODEL_H
#define GSTUFF_MODEL_H

#include "Shaders.h"

#include <string>

namespace GStuff::General {

class Model {
public:
  Model(const std::string& modelPath) 
    : m_FilePath{modelPath} {}
  virtual ~Model() = default;

  using Program = GStuff::General::Shaders::Program;
  virtual void Draw(Program*) = 0;
  
protected:
  std::string m_Path;
};

}

#endif
