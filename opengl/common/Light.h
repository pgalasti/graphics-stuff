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

  struct ShaderConstants {
    // Common
    std::string Ambient   {"ambient"};
    std::string Diffuse   {"diffuse"};
    std::string Specular  {"specular"};

    // Directional
    std::string Direction {"direction"};
    // Add others as I write them
  };

  virtual void Apply(const ShaderConstants& constantNames) = 0;

  // Could use an enum setter but this is more explicit I think
  void SetAmbient(const glm::vec3& ambient)   {m_Attributes.Ambient  = ambient;}
  void SetDiffuse(const glm::vec3& diffuse)   {m_Attributes.Diffuse  = diffuse;}
  void SetSpecular(const glm::vec3& specular) {m_Attributes.Specular = specular;}
  void SetAttributes(const LightAttributes& attributes) { m_Attributes = attributes; }
protected:

  OpenGLProgram* m_pProgram;
  std::string m_LightName;

  LightAttributes m_Attributes;
};

class DirectionalLight : public Light {
public:
  DirectionalLight(OpenGLProgram* pProgram, const std::string& constantName, const glm::vec3& direction) : Light(pProgram, constantName), m_Direction{direction} {}
  ~DirectionalLight() {}

  void Apply(const ShaderConstants& constantNames = {}) override;

  void SetDirection(const glm::vec3& direction) {
    m_Direction = glm::normalize(direction);
  }

private:
  glm::vec3 m_Direction;

};

}

#endif
