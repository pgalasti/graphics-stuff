#ifndef G_HELPER_H
#define G_HELPER_H

#include"defines.h"

#include<iostream>
#include<fstream>

namespace GStuff::Helper {

bool isShaderCompileSuccessful(const ObjID shaderId) {
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

bool isProgramLinkSuccessful(const ObjID programId) {
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

std::string LoadFileStr(const std::string& path) {
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

}

#endif
