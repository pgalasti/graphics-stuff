#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "general/Math.h"
#include "general/ModelLoader.h"

#include "opengl/common/defines.h"
#include "opengl/common/Helper.h"
#include "opengl/common/OpenGLShaders.h"

#include <iostream>
#include <string>
#include <string_view>
#include <fstream>

using namespace GStuff::OpenGL;
using namespace GStuff::OpenGL::Helper;
using namespace GStuff::General;
using namespace GStuff::General::Shaders;
using namespace GStuff::General::Math;

void handleInput(GLFWwindow* pWindow);

int main([[maybe_unused]]int argc, [[maybe_unused]]char* argv[]) {

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
  glfwSetFramebufferSizeCallback(pWindow, []([[maybe_unused]]GLFWwindow* pWindow, int width, int height) { glViewport(0, 0, width, height); });

  glewExperimental = GL_TRUE;
  if(glewInit() != GLEW_OK) {
    std::cerr << "Failed to init GLEW\n";
    return 1;
  }
  std::cout << "OpenGL Version Loaded: " << glGetString(GL_VERSION) << std::endl;

  using ModelLoader = WavefrontVertexLoader<Vertex3Df>;
  ModelLoader modelLoader(ModelLoader::Format::XYZ);
  const auto vertices {modelLoader.Load("./model/test.obj")};
  std::cout << "Loaded " << vertices.size() << " vertices" << std::endl;

  OpenGLProgram program {
    std::make_unique<OpenGLShader>("./shaders/vs.glsl", Shader::ShaderType::Vertex),
    std::make_unique<OpenGLShader>("./shaders/fs.glsl", Shader::ShaderType::Fragment)
  }; 

  ObjID VAO, VBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  
  glBindVertexArray(VAO);
  
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex3Df), vertices.data(), GL_STATIC_DRAW);
 

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D<float>), static_cast<void*>(0));
  glEnableVertexAttribArray(0);

  // Default point size is 1px, which is invisible in practice.
  glPointSize(10.0f);

  while(!glfwWindowShouldClose(pWindow)) {
    handleInput(pWindow);
    glfwPollEvents();

    glClearColor(0.2f, 0.3f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    program.Activate();
    program.SetConstant("triangleColor", glm::vec3{1.0f, 0.0f, 0.0f});

    glBindVertexArray(VAO);
    // No face ('f') parsing yet, so there is no index data -- draw the raw points.
    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(vertices.size()));
    glBindVertexArray(0);

    glfwSwapBuffers(pWindow);
  }

  glfwTerminate();
  return 0;
}

void handleInput(GLFWwindow* pWindow) {
  if(glfwGetKey(pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(pWindow, true);
  }
}
