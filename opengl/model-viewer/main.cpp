#include "general/General.h"
#include "general/Math.h"
#include "general/Profiler.h"
#include "general/ModelLoader.h"

#include "opengl/common/defines.h"
#include "opengl/common/Helper.h"
#include "opengl/common/OpenGLShaders.h"
#include "opengl/common/Texture.h"
#include "opengl/common/OpenGLCamera.h"
#include "opengl/common/Lightsources.h"

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
#include <cstdlib>


using namespace GStuff::OpenGL;
using namespace GStuff::OpenGL::Helper;
using namespace GStuff::General::Shaders;
using namespace GStuff::General::Math;
using namespace GStuff::General;

using ModelLoader = WavefrontVertexLoader<Vertex3DNRGBf>;

void handleInput(GLFWwindow* pWindow);
void mouseEventCallback(GLFWwindow* pWindow, double xPos, double yPos);
void addColor(std::vector<Vertex3DNRGBf>& vertices);

constexpr int WINDOW_WIDTH  {800};
constexpr int WINDOW_HEIGHT {600};
constexpr float NEAR {0.1f};
constexpr float FAR  {1000.0f};
constexpr float ASPECT       {static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT)};
constexpr float ORTHO_HEIGHT {6.0f}; // World units visible vertically

OpenGLCamera camera(0.0f, 0.0f, 3.0f);
FrameStatf g_FrameStat;

float lastX{}, lastY{};

bool doOrtho{false};
bool g_RotateEnabled {false}, g_ReverseRotateEnabled{false};
float g_AccumulatedTime {0.0f};

std::string g_ModelPath;

int main([[maybe_unused]]int argc, [[maybe_unused]]char* argv[]) {

  if(argc < 2) {
    std::cerr << "Usage: ./model-view /path/to/obj/file <optional -o>" << std::endl;
    return 1;
  }

  g_ModelPath = argv[1];
  std::cout << "Loading model: " << g_ModelPath << std::endl;

  if(argc > 2) {
    doOrtho = (*argv[2] == 'o');
    if(doOrtho) {
      std::cout << "Orthogonal Mode enabled!" << std::endl;
    }
  }

#ifndef __APPLE__
  if(std::getenv("GSTUFF_FORCE_X11")) {
    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
  }
#endif
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* pWindow {glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, std::format("Model Viewer: {}", g_ModelPath).c_str(), nullptr, nullptr)};
  if(!pWindow) {
    std::cerr << "Failed to create window!\n";
    glfwTerminate();
    return 1;
  }

  glfwSetInputMode(pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  if(glfwRawMouseMotionSupported()) {
    glfwSetInputMode(pWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
  }
  glfwSetCursorPosCallback(pWindow, mouseEventCallback);

  glfwMakeContextCurrent(pWindow);
  glfwSwapInterval(0); // Attempt to uncap VSync
  glfwSetFramebufferSizeCallback(pWindow, []([[maybe_unused]]GLFWwindow* pWindow, int width, int height) { glViewport(0, 0, width, height); });

  glewExperimental = GL_TRUE;
  if(const GLenum glewStatus {glewInit()};
     glewStatus != GLEW_OK && glewStatus != GLEW_ERROR_NO_GLX_DISPLAY) {
    std::cerr << "Failed to init GLEW: " << glewGetErrorString(glewStatus)
              << " (" << glewStatus << ")\n";
    return 1;
  }

  // On Wayland (and any HiDPI setup) the framebuffer is larger than the window,
  // and the resize callback doesn't fire at startup, so seed the viewport here.
  int frameBufferWidth {}, frameBufferHeight {};
  glfwGetFramebufferSize(pWindow, &frameBufferWidth, &frameBufferHeight);
  glViewport(0, 0, frameBufferWidth, frameBufferHeight);

  glEnable(GL_DEPTH_TEST);
  std::cout << "OpenGL Version Loaded: " << glGetString(GL_VERSION) << std::endl;

  std::cout << "Loading model" << std::endl;
  ProfilerMs profiler;
 
  ModelLoader modelLoader(ModelLoader::Format::XYZNRGB);
  
  profiler.Start("Loading Models");
  const ModelLoader::ModelData modelData = modelLoader.Load(g_ModelPath, ModelLoader::LOAD_INDICES | ModelLoader::LOAD_NORMALS);
  const ModelLoader::ModelData sphereData = modelLoader.Load("./models/sphere.obj", ModelLoader::LOAD_INDICES | ModelLoader::LOAD_NORMALS);
  const auto snapshot {profiler.Stop()};

  ModelLoader::VertexData modelVertices{modelData.first};
  const ModelLoader::IndexData modelIndices{modelData.second};
  ModelLoader::VertexData sphereVertices{sphereData.first};
  const ModelLoader::IndexData sphereIndices{sphereData.second};

  if(modelVertices.empty() || modelIndices.empty() || sphereVertices.empty() || sphereIndices.empty()) {
    throw std::runtime_error("Unable to load vertices/indices of models!");
  }
  addColor(modelVertices);

  std::cout << snapshot << std::endl;
  std::cout << "Loaded " << modelVertices.size() + sphereVertices.size() << " vertices, " << modelIndices.size() + sphereIndices.size() << " indices" << std::endl;

  const std::size_t modelVertexByteSize{modelVertices.size()*sizeof(modelVertices[0])};
  const std::size_t modelIndexByteSize{modelIndices.size()*sizeof(modelIndices[0])};
  const std::size_t sphereVertexByteSize{sphereVertices.size()*sizeof(sphereVertices[0])};
  const std::size_t sphereIndexByteSize{sphereIndices.size()*sizeof(sphereIndices[0])};

  // I need to implement a constructor without perfect forwarding to reuse a compiled shader..
  OpenGLProgram objectProgram {
    std::make_unique<OpenGLShader>("./shaders/vs.glsl", Shader::ShaderType::Vertex),
    std::make_unique<OpenGLShader>("./shaders/fs.glsl", Shader::ShaderType::Fragment)
  };
  OpenGLProgram lightProgram {
    std::make_unique<OpenGLShader>("./shaders/vs.glsl", Shader::ShaderType::Vertex),
    std::make_unique<OpenGLShader>("./shaders/fs-light.glsl", Shader::ShaderType::Fragment)
  };

  ObjID sphereVAO, modelVAO, 
	sphereVBO, modelVBO,
	sphereEBO, modelEBO; 
  glGenVertexArrays(1, &sphereVAO); glGenVertexArrays(1, &modelVAO);
  glGenBuffers(1, &sphereVBO); glGenBuffers(1, &modelVBO);
  glGenBuffers(1, &sphereEBO); glGenBuffers(1, &modelEBO);
 
  // Setup Model VAO 
  glBindVertexArray(modelVAO);
  glBindBuffer(GL_ARRAY_BUFFER, modelVBO);
  glBufferData(GL_ARRAY_BUFFER, modelVertexByteSize, modelVertices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, modelEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, modelIndexByteSize, modelIndices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3DNRGBf), static_cast<void*>(0));
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3DNRGBf), (void*)(sizeof(float)*3));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3DNRGBf), (void*)(sizeof(float)*6));
  glEnableVertexAttribArray(2);

  // Setup Sphere VAO
  glBindVertexArray(sphereVAO);
  glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
  glBufferData(GL_ARRAY_BUFFER, sphereVertexByteSize, sphereVertices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndexByteSize, sphereIndices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3DNRGBf), static_cast<void*>(0));
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3DNRGBf), (void*)(sizeof(float)*3));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3DNRGBf), (void*)(sizeof(float)*6));
  glEnableVertexAttribArray(2);

  camera.Target(0.0f, 0.0f, 0.0f);
  camera.Up(0.0f, 1.0f, 0.0f);

  if(doOrtho) { 
    constexpr float orthoHalfHeight {ORTHO_HEIGHT*0.5f};
    constexpr float orthoHalfWidth  {orthoHalfHeight*ASPECT};
    camera.SetOrthogonal(-orthoHalfWidth, orthoHalfWidth, -orthoHalfHeight, orthoHalfHeight, NEAR, FAR);
  } else {
    camera.SetPerspective(glm::radians(45.0f), ASPECT, NEAR, FAR);
  }

  const glm::mat4 projection { camera.Projection() };
  objectProgram.SetConstant("projection", projection);
  lightProgram.SetConstant("projection", projection);

  Light sphereLightAttributes {
    glm::vec3(0.1f, 0.10f, 0.1f),
    glm::vec3(0.50f, 0.50f, 0.5f),
    glm::vec3(1.0f, 1.0f, 1.0f)
  };

  LightSources lightSources(&objectProgram);

  double deltaFrameTime { 0.0 }, 
	 lastTime {glfwGetTime()};
  while(!glfwWindowShouldClose(pWindow)) {
    deltaFrameTime = glfwGetTime() - lastTime;
    lastTime = glfwGetTime(); 
    g_FrameStat.update(glfwGetTime());
    
    handleInput(pWindow);
    glfwPollEvents();

    glClearColor(0.2f, 0.3f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
    const glm::mat4 lookAt { camera.ApplyView() };
    
    // Setup Sphere 
    glm::mat4 lightModelMtx {glm::mat4(1.0f)};
    lightModelMtx = glm::translate(lightModelMtx, glm::vec3(0.0f, 1.0f, 0.0f));
    lightModelMtx = glm::rotate(lightModelMtx, static_cast<float>(glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f));
    lightModelMtx = glm::translate(lightModelMtx, glm::vec3(1.0f, 0.0f, 0.0f));
    lightModelMtx = glm::scale(lightModelMtx, glm::vec3(0.25f, 0.25f, 0.25f));
    
    // lightModelMtx should be the 4th column that contains the translation values so I'll set that as the light position
    // in the shader
    const glm::vec3 lightPosition {lightModelMtx[3]};

    // Setup Model 
    glm::mat4 modelMtx {glm::mat4(1.0f)};
    modelMtx = glm::translate(modelMtx, glm::vec3(0.0f, 0.0f, 0.0f));
    if(g_RotateEnabled) {
      g_AccumulatedTime += static_cast<float>(deltaFrameTime);
    } else if(g_ReverseRotateEnabled) {
      g_AccumulatedTime -= static_cast<float>(deltaFrameTime);
    }
    modelMtx = glm::rotate(modelMtx, g_AccumulatedTime, glm::vec3(1.0f, 1.0f, 1.0f));
    modelMtx = glm::scale(modelMtx, glm::vec3(0.25f, 0.25f, 0.25f));
    const glm::mat3 inverseTransposeMtx { glm::transpose(glm::inverse(modelMtx)) };

    // Setup matrix constants
    objectProgram.SetConstant("model", modelMtx);
    objectProgram.SetConstant("view", lookAt);
    objectProgram.SetConstant("inverseTransposeMtx", inverseTransposeMtx);

    // Light stuff
    lightSources.SetCameraPosition("cameraPosition", camera.Position());
    lightSources.SetProperty("light.position", lightPosition);
    lightSources.SetProperty("light.ambient", sphereLightAttributes.ambient);
    lightSources.SetProperty("light.diffuse", sphereLightAttributes.diffuse);
    lightSources.SetProperty("light.specular", sphereLightAttributes.specular);
    lightSources.Apply();

    // Material stuff
    objectProgram.SetConstant("material.ambient", glm::vec3(1.0f, 0.5f, 0.31f));
    objectProgram.SetConstant("material.diffuse", glm::vec3(1.0f, 0.5f, 0.31f));
    objectProgram.SetConstant("material.specular", glm::vec3(0.50f, 0.5f, 0.5f));
    objectProgram.SetConstant("material.shine", 32.0f);
    objectProgram.Activate();
    
    // Draw Model
    glBindVertexArray(modelVAO);
    glDrawElements(GL_TRIANGLES, modelIndices.size(), GL_UNSIGNED_INT, 0);

    // Draw Sphere 
    lightProgram.SetConstant("model", lightModelMtx);
    lightProgram.SetConstant("view", lookAt);
    lightProgram.SetConstant("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
    lightProgram.SetConstant("intensity", 1.0f);
    lightProgram.Activate();
    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);

    glfwSwapBuffers(pWindow);
  }
  
  glDeleteVertexArrays(1, &modelVAO);
  glDeleteVertexArrays(1, &sphereVAO);
  glDeleteBuffers(1, &modelVBO);
  glDeleteBuffers(1, &sphereVBO);
  glDeleteBuffers(1, &modelEBO);
  glDeleteBuffers(1, &sphereEBO);
  
  // Wayland+NVIDIA segfaults in an internal function wl_display_disconnect() 
  // on my nvidia drivers so i have this ugly code 
  if(glfwGetPlatform() != GLFW_PLATFORM_WAYLAND) {
    glfwTerminate();
  }

  return 0;
}

void handleInput(GLFWwindow* pWindow) {
  if(glfwGetKey(pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(pWindow, true);
  }

  constexpr float speed {2.50f};
  float cameraSpeed {speed*g_FrameStat.DeltaTime};
  if(glfwGetKey(pWindow, GLFW_KEY_W) == GLFW_PRESS) {
    camera.Move(CameraDirection::Forward, cameraSpeed);
  } 
  if(glfwGetKey(pWindow, GLFW_KEY_S) == GLFW_PRESS) {
    camera.Move(CameraDirection::Backward, cameraSpeed);
  } 
  if(glfwGetKey(pWindow, GLFW_KEY_A) == GLFW_PRESS) {
    camera.Move(CameraDirection::Left, cameraSpeed);
  } 
  if(glfwGetKey(pWindow, GLFW_KEY_D) == GLFW_PRESS) {
    camera.Move(CameraDirection::Right, cameraSpeed);
  } 
  if(glfwGetKey(pWindow, GLFW_KEY_SPACE) == GLFW_PRESS) {
    camera.Move(CameraDirection::Up, cameraSpeed);
  } 
  if(glfwGetKey(pWindow, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
    camera.Move(CameraDirection::Down, cameraSpeed);
  }

  if(glfwGetKey(pWindow, GLFW_KEY_1) == GLFW_PRESS) {
    g_RotateEnabled = true;
  } else {
    g_RotateEnabled = false;
  }
  if(glfwGetKey(pWindow, GLFW_KEY_2) == GLFW_PRESS) {
    g_ReverseRotateEnabled = true;
  } else {
    g_ReverseRotateEnabled = false;
  }

}
void mouseEventCallback([[maybe_unused]]GLFWwindow* pWindow, double xPos, double yPos) {

  static bool firstMouse{true};
  if(firstMouse) {
    lastX = xPos;
    lastY = yPos;
    firstMouse = false;
  }

  float xOffset {static_cast<float>(xPos - lastX)};
  float yOffset {static_cast<float>(lastY - yPos)}; // Reversed: screen y grows downward

  lastX = xPos;
  lastY = yPos;

  constexpr float sensitivity {0.1f};
  xOffset *= sensitivity;
  yOffset *= sensitivity;

  camera.Rotate(xOffset, yOffset);
  //camera.Rotate(OpenGLCamera::Rotation::Pitch, yOffset);
  //camera.Rotate(OpenGLCamera::Rotation::Yaw, xOffset);

}

void addColor(std::vector<Vertex3DNRGBf>& vertices) {
  for(auto& vertex : vertices) {
    vertex.r = 1.0f;
    vertex.g = 0.0f;
    vertex.b = 0.0f;
  }
}
