#ifndef G_OPENGL_SHADERS_H
#define G_OPENGL_SHADERS_H

#include "general/Shaders.h"
#include "opengl/common/defines.h"

#include <string>

namespace GStuff::OpenGL {

using GStuff::General::Shaders::Shader;

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

}
#endif
