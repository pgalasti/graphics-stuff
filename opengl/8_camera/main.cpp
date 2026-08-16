#include "general/Math.h"
#include "general/Profiler.h"
#include "general/ModelLoader.h"

#include "opengl/common/defines.h"
#include "opengl/common/Helper.h"
#include "opengl/common/OpenGLShaders.h"
#include "opengl/common/Texture.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>
#include <string_view>
#include <fstream>
#include <cstddef>
#include <random>


using namespace GStuff::OpenGL;
using namespace GStuff::OpenGL::Helper;
using namespace GStuff::General::Shaders;
using namespace GStuff::General::Math;
using namespace GStuff::General;

using ModelLoader = WavefrontVertexLoader<Vertex3DRGBf>;


void handleInput(GLFWwindow* pWindow);
void addColors(ModelLoader::VertexData& vertices);

constexpr int WINDOW_WIDTH  {800};
constexpr int WINDOW_HEIGHT {600};

int main([[maybe_unused]]int argc, [[maybe_unused]]char* argv[]) {

#ifndef __APPLE__
  // Wayland stuff makes life hard
  glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
#endif
  glfwInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* pWindow {glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Camera", nullptr, nullptr)};
  if(!pWindow) {
    std::cerr << "Failed to create window!\n";
    glfwTerminate();
    return 1;
  }

  glfwMakeContextCurrent(pWindow);
  glfwSwapInterval(0); // Attempt to uncap VSync
  glfwSetFramebufferSizeCallback(pWindow, []([[maybe_unused]]GLFWwindow* pWindow, int width, int height) { glViewport(0, 0, width, height); });

  glewExperimental = GL_TRUE;
  if(glewInit() != GLEW_OK) {
    std::cerr << "Failed to init GLEW\n";
    return 1;
  }

  glEnable(GL_DEPTH_TEST);
  std::cout << "OpenGL Version Loaded: " << glGetString(GL_VERSION) << std::endl;

  std::cout << "Loading model" << std::endl;
  ProfilerMs profiler;
 
  ModelLoader modelLoader(ModelLoader::Format::XYZRGB);
  
  profiler.Start("Loading Model");
  const ModelLoader::ModelData modelData = modelLoader.Load("./models/cube.obj", ModelLoader::LOAD_INDICES);
  const auto snapshot {profiler.Stop()};
  ModelLoader::VertexData vertices{modelData.first};
  const ModelLoader::IndexData indices{modelData.second};
  if(vertices.empty() || indices.empty()) {
    throw std::runtime_error("Unable to load vertices/indices of model!");
  }

  addColors(vertices);
  std::cout << snapshot << std::endl;

  const std::size_t vertexByteSize{vertices.size()*sizeof(vertices[0])};
  const std::size_t indexByteSize{indices.size()*sizeof(indices[0])};

  OpenGLProgram program {
    std::make_unique<OpenGLShader>("./shaders/vs.glsl", Shader::ShaderType::Vertex),
    std::make_unique<OpenGLShader>("./shaders/fs.glsl", Shader::ShaderType::Fragment)
  }; 

  ObjID VAO, VBO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
  
  glBindVertexArray(VAO);
  
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertexByteSize, vertices.data(), GL_STATIC_DRAW);
 
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexByteSize, indices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3DRGBf), static_cast<void*>(0));
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3DRGBf), (void*)(sizeof(float)*3));
  glEnableVertexAttribArray(1);

  glm::vec3 cameraPosition { glm::vec3(0.0f, 0.0f, 3.0f) };
  glm::vec3 cameraTarget   { glm::vec3(0.0f, 0.0f, 0.0f) };
  glm::vec3 cameraDirection{ glm::normalize(cameraPosition - cameraTarget) };
  const glm::vec3 up       { glm::vec3(0.0f, 1.0f, 0.0f) };
  glm::vec3 cameraRight    { glm::normalize(glm::cross(up, cameraDirection)) };
  glm::vec3 cameraUp       { glm::normalize(glm::cross(cameraDirection, cameraRight)) };
  glm::mat4 lookAt {
    glm::lookAt(
      cameraPosition,
      cameraTarget,
      up
    )
  };

  program.Activate();

  constexpr float radius {10.0f};

  while(!glfwWindowShouldClose(pWindow)) {
    handleInput(pWindow);
    glfwPollEvents();

    glClearColor(0.2f, 0.3f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glm::mat4 modelMtx {glm::mat4(1.0f)};
    modelMtx = glm::scale(modelMtx, glm::vec3(0.25f, 0.25f, 0.25f));
    modelMtx = glm::rotate(modelMtx, static_cast<float>(glfwGetTime()), glm::vec3(1.0f, 1.0f, 1.0f));
    SetTransformation(program.ProgramID(), "model", modelMtx);

    float camX {static_cast<float>(std::sin(glfwGetTime()))*radius}, 
	  camZ {static_cast<float>(std::cos(glfwGetTime()))*radius};
    lookAt = glm::lookAt(
      glm::vec3(camX, 0.0f, camZ),
      cameraTarget,
      up
    );

    SetTransformation(program.ProgramID(), "view", lookAt);

    glm::mat4 projMtx {glm::perspective(glm::radians(45.0f), static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT), 0.1f, 1000.0f)}; 
    SetTransformation(program.ProgramID(), "projection", projMtx);
     
    glBindVertexArray(VAO);
    program.Activate();

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    // Draw a few more for perspective, why not
    modelMtx = glm::mat4(1.0f);
    modelMtx = glm::translate(modelMtx, glm::vec3(-2.0f, 0.0f, -5.0f));
    modelMtx = glm::scale(modelMtx, glm::vec3(0.25f, 0.25f, 0.25f));
    modelMtx = glm::rotate(modelMtx, static_cast<float>(glfwGetTime()), glm::vec3(1.0f, 1.0f, 1.0f));
    SetTransformation(program.ProgramID(), "model", modelMtx);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    
    modelMtx = glm::mat4(1.0f);
    modelMtx = glm::translate(modelMtx, glm::vec3(2.0f, 0.0f, -1.0f));
    modelMtx = glm::scale(modelMtx, glm::vec3(0.25f, 0.25f, 0.25f));
    modelMtx = glm::rotate(modelMtx, static_cast<float>(glfwGetTime()), glm::vec3(1.0f, 1.0f, 1.0f));
    SetTransformation(program.ProgramID(), "model", modelMtx);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    modelMtx = glm::mat4(1.0f);
    modelMtx = glm::translate(modelMtx, glm::vec3(1.0f, 3.0f, -20.0f));
    modelMtx = glm::scale(modelMtx, glm::vec3(0.25f, 0.25f, 0.25f));
    modelMtx = glm::rotate(modelMtx, static_cast<float>(glfwGetTime()), glm::vec3(1.0f, 1.0f, 1.0f));
    SetTransformation(program.ProgramID(), "model", modelMtx);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    
    
    
    glBindVertexArray(0);

    glfwSwapBuffers(pWindow);
  }
  
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glfwTerminate();
  return 0;
}

void handleInput(GLFWwindow* pWindow) {
  if(glfwGetKey(pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(pWindow, true);
  }
}

void addColors(ModelLoader::VertexData& vertices) {

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> distribution(0.0f, 1.0f);


  for(auto& vertex : vertices) {
    vertex.r = distribution(gen);
    vertex.g = distribution(gen);
    vertex.b = distribution(gen);
  }
}
