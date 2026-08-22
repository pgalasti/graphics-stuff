#ifndef G_HELPER_H
#define G_HELPER_H

#include"defines.h"

#include<GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include<iostream>
#include<fstream>
#include<string>
#include<string_view>

namespace GStuff::OpenGL::Helper {

struct Light {
  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;
};

inline bool isShaderCompileSuccessful(const ObjID shaderId, std::string_view label = {}) {
  int success{};

  glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
  if(!success) {
    int logLength{};
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);

    std::string infoLog(logLength > 0 ? logLength : 1, '\0');
    glGetShaderInfoLog(shaderId, static_cast<GLsizei>(infoLog.size()), nullptr, infoLog.data());

    std::cerr << "Error compiling shader";
    if(!label.empty()) std::cerr << " '" << label << "'";
    std::cerr << ":\n" << infoLog.c_str() << std::endl;
    return false;
  }

  return true;
}

inline bool isProgramLinkSuccessful(const ObjID programId, std::string_view label = {}) {
  int success{};

  glGetProgramiv(programId, GL_LINK_STATUS, &success);
  if(!success) {
    int logLength{};
    glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logLength);

    std::string infoLog(logLength > 0 ? logLength : 1, '\0');
    glGetProgramInfoLog(programId, static_cast<GLsizei>(infoLog.size()), nullptr, infoLog.data());

    std::cerr << "Error linking shader program";
    if(!label.empty()) std::cerr << " '" << label << "'";
    std::cerr << ":\n" << infoLog.c_str() << std::endl;
    return false;
  }

  return true;
}

inline std::string LoadFileStr(const std::string& path) {
  std::ifstream file;

  file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  file.open(path);
  file.exceptions(std::ifstream::badbit);

  std::string fileContents;
  std::string line;
  while(std::getline(file, line)) {
    fileContents += line+"\n";
  }

  return fileContents;
}

inline void SetTransformation(const ObjID programID, std::string transformName, const glm::mat4& transformation) {
  const GLint response { glGetUniformLocation(programID, transformName.c_str()) };
  if(response < 0) {
    throw std::runtime_error("Received invalid response upon setting transformation matrix with glGetUniformLocation");
  }

  glUniformMatrix4fv(response, 1, GL_FALSE, glm::value_ptr(transformation)); 
}
  
}

#endif
