#ifndef G_HELPER_H
#define G_HELPER_H

#include"defines.h"

#include<GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include<iostream>
#include<fstream>
#include<string>

namespace GStuff::OpenGL::Helper {

inline bool isShaderCompileSuccessful(const ObjID shaderId) {
  int success{};
  char infoLog[512];

  glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
  if(!success) {
    glGetShaderInfoLog(shaderId, 512, nullptr, infoLog);
    std::cerr << "Error on shader compilation: " << infoLog << std::endl;
    return false;
  }

  return true;
}

inline bool isProgramLinkSuccessful(const ObjID programId) {
  int success{};
  char infoLog[512];

  glGetProgramiv(programId, GL_LINK_STATUS, &success);
  if(!success) {
    glGetProgramInfoLog(programId, 512, nullptr, infoLog);
    std::cerr << "Error on shader program linking: " << infoLog << std::endl;
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
