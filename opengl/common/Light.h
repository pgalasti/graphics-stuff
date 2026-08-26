#ifndef GSTUFF_LIGHT_H
#define GSTUFF_LIGHT_H

#include "OpenGLShaders.h"

#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace GStuff::OpenGL {

// I can make these more general with templates later instead of
// explicitly using glm


struct LightAttributes {
  glm::vec3 Ambient;
  glm::vec3 Diffuse;
  glm::vec3 Specular;
};

class Light {
public:
  Light(OpenGLProgram* pProgram, const std::string& constantName) : m_pProgram{pProgram}, m_LightName{constantName} {};
  virtual ~Light() = default;

  virtual Apply() = 0;

protected:

  OpenGLProgram* m_pProgram;
  std::string m_LightName;

  union {
    LightAttributes attributes;
    struct {
      glm::vec3 m_Ambient;
      glm::vec3 m_Diffuse;
      glm::vec3 m_Specular;
    };
  };
};

}

#endif
