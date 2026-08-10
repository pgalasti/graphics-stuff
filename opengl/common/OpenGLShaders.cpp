#include <GL/glew.h>

#include "OpenGLShaders.h"
#include "Helper.h"

#include <stdexcept>
#include <iostream>
#include <cstdlib>

namespace GStuff::OpenGL {

using namespace GStuff::OpenGL::Helper;

OpenGLShader::OpenGLShader(const std::string& path, Shader::ShaderType type)
  : Shader(path, type), m_ShaderID(0) {

  try {
    m_ShaderContents = LoadFileStr(path);
  } catch(const std::ios_base::failure& ex) {
    std::cerr << "Error loading shader contents: " << ex.what() << std::endl;
    std::exit(EXIT_FAILURE);
  }

  GLuint shaderType {};
  switch(type) {
    case Shader::ShaderType::Vertex:
      shaderType = GL_VERTEX_SHADER;
      break;
    case Shader::ShaderType::Fragment:
      shaderType = GL_FRAGMENT_SHADER;
      break;
  }

  m_ShaderID = glCreateShader(shaderType);
  const char* codePtr {m_ShaderContents.c_str()};
  glShaderSource(m_ShaderID, 1, &codePtr, nullptr);
  glCompileShader(m_ShaderID);
  if(!isShaderCompileSuccessful(m_ShaderID)) {
    std::exit(EXIT_FAILURE);
  }

}

OpenGLShader::~OpenGLShader() {
  glDeleteShader(m_ShaderID);
}

std::string OpenGLShader::ReadShaderText() const {
  return m_ShaderContents;
}

}
