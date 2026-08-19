#ifndef G_OPENGL_SHADERS_H
#define G_OPENGL_SHADERS_H

#include "general/Shaders.h"
#include "opengl/common/defines.h"

#include <concepts>
#include <string>
#include <utility>

namespace GStuff::OpenGL {

using GStuff::General::Shaders::Shader;
using GStuff::General::Shaders::Program;
using GStuff::General::Shaders::ShaderPtr;

class OpenGLShader : public Shader {
public:
  OpenGLShader(const std::string& path, Shader::ShaderType type);
  ~OpenGLShader() override;

  std::string ReadShaderText() const override;

  ObjID ShaderID() const override {
    return m_ShaderID;
  }

private:
  ObjID m_ShaderID;
  std::string m_ShaderContents;
};

class OpenGLProgram : public Program {
public:
  template <std::convertible_to<ShaderPtr>... Shaders>
  explicit OpenGLProgram(Shaders&&... shaders)
    : Program(std::forward<Shaders>(shaders)...) {
    Link();
  }

  ~OpenGLProgram() override;

  void Activate(bool active = true) override;

  ObjID ProgramID() const {
    return m_ProgramID;
  }

  void SetConstant(const std::string& name, const glm::vec3& vec) override;
  void SetConstant(const std::string& name, int value) override;
  void SetConstant(const std::string& name, float value) override;
private:
  void Link();

  ObjID m_ProgramID {};
};

}
#endif
