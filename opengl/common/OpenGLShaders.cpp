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
  if(!isShaderCompileSuccessful(m_ShaderID, path)) {
    std::exit(EXIT_FAILURE);
  }

}

OpenGLShader::~OpenGLShader() {
  glDeleteShader(m_ShaderID);
}

std::string OpenGLShader::ReadShaderText() const {
  return m_ShaderContents;
}

void OpenGLProgram::Link() {
  m_ProgramID = glCreateProgram();
  for(const ShaderPtr& shader : m_Shaders) {
    glAttachShader(m_ProgramID, shader->ShaderID());
  }

  glLinkProgram(m_ProgramID);

  std::string label;
  for(const ShaderPtr& shader : m_Shaders) {
    if(!label.empty()) label += " + ";
    label += shader->Path();
  }

  if(!isProgramLinkSuccessful(m_ProgramID, label)) {
    std::exit(EXIT_FAILURE);
  }

  for(const ShaderPtr& shader : m_Shaders) {
    glDetachShader(m_ProgramID, shader->ShaderID());
  }
  m_Shaders.clear();
}

OpenGLProgram::~OpenGLProgram() {
  glDeleteProgram(m_ProgramID);
}

void OpenGLProgram::Activate(bool active) {
  glUseProgram(active ? m_ProgramID : 0);
}

namespace {

int uniformLocationOrWarn(ObjID programID, const std::string& name) {
  int location {glGetUniformLocation(programID, name.c_str())};
  if(location == -1) {
    std::cerr << "Uniform '" << name << "' not found in program " << programID
              << " (misspelled, or optimized out because it is unused)\n";
  }
  return location;
}

}

void OpenGLProgram::SetConstant(const std::string& name, const glm::vec3& vec) {
  glUniform3f(uniformLocationOrWarn(m_ProgramID, name), vec.x, vec.y, vec.z);
}

void OpenGLProgram::SetConstant(const std::string& name, int value) {
  glUniform1i(uniformLocationOrWarn(m_ProgramID, name), value);
}

}
