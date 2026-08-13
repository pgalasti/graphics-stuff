#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "common/defines.h"
#include "common/Helper.h"

#include <iostream>
#include <string>

using namespace GStuff::OpenGL::Helper;

void frameBufferSize_callback(GLFWwindow* pWindow, int width, int height);
void handleInput(GLFWwindow* pWindow);

constexpr float vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
};  

const std::string vsSource = 
  "#version 330 core\n"
  "layout (location = 0) in vec3 aPos;\n"
  "void main(){\n"
  "  gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
  "}";


const std::string fsSource =  
  "#version 330 core\n"
  "out vec4 FragColor;\n"
  "void main() {\n"
  "  FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
  "}";


int main(int argc, char* argv[]) {

  // Wayland stuff makes life hard
  glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
  glfwInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* pWindow {glfwCreateWindow(800, 600, "triangle", nullptr, nullptr)};
  if(!pWindow) {
    std::cerr << "Failed to create window!\n";
    glfwTerminate();
    return 1;
  }
  glfwMakeContextCurrent(pWindow);
  glfwSetFramebufferSizeCallback(pWindow, frameBufferSize_callback);

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

  ObjID VAO, VBO;

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  
  // Use it and clean up shader object handlers
  glUseProgram(shaderProgram);
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  glBindVertexArray(VAO);
  
  while(!glfwWindowShouldClose(pWindow)) {
    handleInput(pWindow);
    glfwSwapBuffers(pWindow);
    glfwPollEvents();

    glClearColor(0.2f, 0.3f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 3);
  }

  glfwTerminate();
  return 0;
}


void frameBufferSize_callback(GLFWwindow* pWindow, int width, int height) {
  glViewport(0, 0, width, height);
}


void handleInput(GLFWwindow* pWindow) {
  if(glfwGetKey(pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(pWindow, true);
  }
}

