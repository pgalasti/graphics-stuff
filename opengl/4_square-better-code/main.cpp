#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "general/Math.h"
#include "opengl/common/defines.h"
#include "opengl/common/Helper.h"
#include "opengl/common/OpenGLShaders.h"

#include <iostream>
#include <string>
#include <string_view>
#include <fstream>

using namespace GStuff::OpenGL;
using namespace GStuff::OpenGL::Helper;
using namespace GStuff::General::Shaders;
using namespace GStuff::General::Math;

void handleInput(GLFWwindow* pWindow);

constexpr Vertex3D<float> vertices[] = {
  { .vals = {-0.5f, -0.5f, 0.0f} }, // Bottom Left 
  { .vals = { 0.5f, -0.5f, 0.0f} }, // Bottom Right 
  { .vals = { 0.5f,  0.5f, 0.0f} }, // Top Right 
  { .vals = { -0.5f, 0.5f, 0.0f} } // Top Left 
};

constexpr unsigned int indices[] = {
   0, 1, 2,
   0, 3, 2
};

int main(int argc, char* argv[]) {


  // Wayland stuff makes life hard
  glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
  glfwInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* pWindow {glfwCreateWindow(800, 600, "square", nullptr, nullptr)};
  if(!pWindow) {
    std::cerr << "Failed to create window!\n";
    glfwTerminate();
    return 1;
  }
  glfwMakeContextCurrent(pWindow);
  glfwSetFramebufferSizeCallback(pWindow, [](GLFWwindow* pWindow, int width, int height) { glViewport(0, 0, width, height); });

  glewExperimental = GL_TRUE;
  if(glewInit() != GLEW_OK) {
    std::cerr << "Failed to init GLEW\n";
    return 1;
  }
  std::cout << "OpenGL Version Loaded: " << glGetString(GL_VERSION) << std::endl;

  OpenGLShader vertexShader("./shaders/vs.glsl", Shader::ShaderType::Vertex); 
  OpenGLShader fragmentShader("./shaders/fs.glsl", Shader::ShaderType::Fragment); 
  ObjID vsID {vertexShader.ShaderID()};
  ObjID fsID {fragmentShader.ShaderID()};

  // Create the program
  ObjID shaderProgram { glCreateProgram() };
  glAttachShader(shaderProgram, vsID) ;
  glAttachShader(shaderProgram, fsID);
  glLinkProgram(shaderProgram);
  if(!isProgramLinkSuccessful(shaderProgram)) {
    return 1;
  }
  
  ObjID VAO, VBO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
  
  glBindVertexArray(VAO);
  
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
 
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D<float>), static_cast<void*>(0));
  glEnableVertexAttribArray(0);
  
  while(!glfwWindowShouldClose(pWindow)) {
    handleInput(pWindow);
    glfwSwapBuffers(pWindow);
    glfwPollEvents();
  
    glClearColor(0.2f, 0.3f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
  }

  glfwTerminate();
  return 0;
}

void handleInput(GLFWwindow* pWindow) {
  if(glfwGetKey(pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(pWindow, true);
  }
}
