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
    std::string Ambient       {"ambient"};
    std::string Diffuse       {"diffuse"};
    std::string Specular      {"specular"};

    // Directional
    std::string Direction     {"direction"};

    // Attenuation
    std::string Position      {"position"};
    std::string Att_Constant  {"constant"};
    std::string Att_Linear    {"linear"};
    std::string Att_Quadratic {"quadratic"};
    // Add others as I write them
  };

  virtual void Apply(const ShaderConstants& constantNames) = 0;
  virtual void Update() = 0;

  // Could use an enum setter but this is more explicit I think
  void SetAmbient(const glm::vec3& ambient)   {m_Attributes.Ambient  = ambient;}
  void SetDiffuse(const glm::vec3& diffuse)   {m_Attributes.Diffuse  = diffuse;}
  void SetSpecular(const glm::vec3& specular) {m_Attributes.Specular = specular;}
  void SetAttributes(const LightAttributes& attributes) { m_Attributes = attributes; }
protected:

  OpenGLProgram* m_pProgram;
  std::string m_LightName;

  LightAttributes m_Attributes;
  ShaderConstants m_ShaderConstants;
};

class DirectionalLight : public Light {
public:
  DirectionalLight(OpenGLProgram* pProgram, const std::string& constantName, const glm::vec3& direction) : Light(pProgram, constantName), m_Direction{direction} {}
  ~DirectionalLight() {}

  void Apply(const ShaderConstants& constantNames = {}) override;
  void Update() override;

  void SetDirection(const glm::vec3& direction) {
    m_Direction = glm::normalize(direction);
  }

private:
  glm::vec3 m_Direction;

};

class AttenuationLight : public Light {
public:
  AttenuationLight(OpenGLProgram* pProgram, const std::string& constantName, const glm::vec3& position) : Light(pProgram, constantName), m_Position{position} {}
  virtual ~AttenuationLight() = default;

  virtual void Apply(const ShaderConstants& constantNames) = 0;
  virtual void Update() = 0;

  void SetPosition(const glm::vec3& position)   {m_Position = position;}
  void SetAttenuation(const float constant, const float linear, const float quadratic) {
    m_Constant = constant;
    m_Linear = linear;
    m_Quadratic = quadratic;
  }

protected:
  glm::vec3 m_Position;
  float m_Constant;
  float m_Linear;
  float m_Quadratic;
};

class PointLight : public AttenuationLight {
public:
  PointLight(OpenGLProgram* pProgram, const std::string& constantName, const glm::vec3& position) : AttenuationLight(pProgram, constantName, position) {}
  ~PointLight() = default;

  void Apply(const ShaderConstants& constantNames) override;
  void Update() override;
private:
};

}
#endif
