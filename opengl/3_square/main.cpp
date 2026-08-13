#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "general/Math.h"
#include "opengl/common/defines.h"
#include "opengl/common/Helper.h"

#include <iostream>
#include <string>
#include <string_view>
#include <fstream>

using namespace GStuff::OpenGL::Helper;
using namespace GStuff::General::Math;
using namespace GStuff::General;

//void frameBufferSize_callback(GLFWwindow* pWindow, int width, int height);
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

  std::string vsSource, fsSource;
  try { 
    vsSource = LoadFileStr("./shaders/vs.glsl"); 
    fsSource = LoadFileStr("./shaders/fs.glsl"); 
  } catch(const std::ios_base::failure& ex) {
    std::cerr << "Error loading shader contents: " << ex.what() << std::endl;
    return 1;
  }

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


  // Vertex Shader 
  ObjID vertexShader { glCreateShader(GL_VERTEX_SHADER) };
  const char* vsCodePtr { vsSource.c_str() };
  glShaderSource(vertexShader, 1, &vsCodePtr, nullptr);
  glCompileShader(vertexShader);
  if(!isShaderCompileSuccessful(vertexShader)) {
    return 1;
  }

  // Fragment shader
  ObjID fragmentShader { glCreateShader(GL_FRAGMENT_SHADER) };

  const char* fsCodePtr { fsSource.c_str() };
  glShaderSource(fragmentShader, 1, &fsCodePtr, nullptr);
  glCompileShader(fragmentShader);
  if(!isShaderCompileSuccessful(fragmentShader)) {
    return 1;
  }

  // Create the program
  ObjID shaderProgram { glCreateProgram() };
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  if(!isProgramLinkSuccessful(shaderProgram)) {
    return 1;
  }

  // Use it and clean up shader object handlers
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  
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
