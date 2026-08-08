#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

void frameBufferSize_callback(GLFWwindow* pWindow, int width, int height);
void handleInput(GLFWwindow* pWindow);

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

  while(!glfwWindowShouldClose(pWindow)) {
    handleInput(pWindow);
    glfwSwapBuffers(pWindow);
    glfwPollEvents();
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
