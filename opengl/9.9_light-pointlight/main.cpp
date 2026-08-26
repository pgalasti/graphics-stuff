#include "general/General.h"
#include "general/Math.h"
#include "general/Profiler.h"
#include "general/ModelLoader.h"

#include "opengl/common/defines.h"
#include "opengl/common/Helper.h"
#include "opengl/common/OpenGLShaders.h"
#include "opengl/common/Texture.h"
#include "opengl/common/OpenGLCamera.h"
#include "opengl/common/Light.h"

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

using CubeModelLoader = WavefrontVertexLoader<Vertex3DNUVf>;

void handleInput(GLFWwindow* pWindow);
void mouseEventCallback(GLFWwindow* pWindow, double xPos, double yPos);
void addColor(std::vector<Vertex3DNRGBf>& vertices);

constexpr int WINDOW_WIDTH  {800};
constexpr int WINDOW_HEIGHT {600};
constexpr float NEAR {0.1f};
constexpr float FAR  {1000.0f};
constexpr float ASPECT       {static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT)};
constexpr float ORTHO_HEIGHT {6.0f}; // World units visible vertically

constexpr GLuint CONTAINER_UNIT      {0};
constexpr GLuint CONTAINER_EDGE_UNIT {1};

OpenGLCamera camera(0.0f, 0.0f, 3.0f);
FrameStatf g_FrameStat;

float lastX{}, lastY{};

bool doOrtho{false};

int main([[maybe_unused]]int argc, [[maybe_unused]]char* argv[]) {

  if(argc > 1) {
    doOrtho = (*argv[1] == 'o');
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

  GLFWwindow* pWindow {glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Light", nullptr, nullptr)};
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
 
  CubeModelLoader cubeLoader(CubeModelLoader::Format::XYZNUV);
  
  profiler.Start("Loading Models");
  const CubeModelLoader::ModelData cubeData = cubeLoader.Load("./models/cube_unit_uv.obj", CubeModelLoader::LOAD_INDICES | CubeModelLoader::LOAD_NORMALS | CubeModelLoader::LOAD_UV);
  const auto snapshot {profiler.Stop()};

  CubeModelLoader::VertexData cubeVertices{cubeData.first};
  const CubeModelLoader::IndexData cubeIndices{cubeData.second};

  if(cubeVertices.empty() || cubeIndices.empty()) {
    throw std::runtime_error("Unable to load vertices/indices of models!");
  }
  //addColor(cubeVertices);

  std::cout << snapshot << std::endl;
  std::cout << "Loaded " << cubeVertices.size() << " vertices, " << cubeIndices.size() << " indices" << std::endl;

  const std::size_t cubeVertexByteSize{cubeVertices.size()*sizeof(cubeVertices[0])};
  const std::size_t cubeIndexByteSize{cubeIndices.size()*sizeof(cubeIndices[0])};

  // I need to implement a constructor without perfect forwarding to reuse a compiled shader..
  OpenGLProgram objectProgram {
    std::make_unique<OpenGLShader>("./shaders/vs.glsl", Shader::ShaderType::Vertex),
    std::make_unique<OpenGLShader>("./shaders/fs.glsl", Shader::ShaderType::Fragment)
  };

  ObjID cubeVAO, 
	cubeVBO,
	cubeEBO; 
  glGenVertexArrays(1, &cubeVAO);
  glGenBuffers(1, &cubeVBO);
  glGenBuffers(1, &cubeEBO);
 
  // Setup Cube VAO 
  glBindVertexArray(cubeVAO);
  glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
  glBufferData(GL_ARRAY_BUFFER, cubeVertexByteSize, cubeVertices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, cubeIndexByteSize, cubeIndices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3DNUVf), static_cast<void*>(0));
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3DNUVf), (void*)(sizeof(float)*3));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3DNUVf), (void*)(sizeof(float)*6));
  glEnableVertexAttribArray(2);

  profiler.Start("Load Textures");
  
  int width, height, nChannels;
  stbi_set_flip_vertically_on_load(true);
  TextureData* textureData {stbi_load("./textures/container2.png", &width, &height, &nChannels, 0)};
  if(!textureData) {
    throw std::runtime_error("Unable to load texture data from file system!");
  }
  Texture containerTexture(textureData, width, height, GL_RGBA);
  stbi_image_free(textureData);

  textureData = stbi_load("./textures/container2_specular.png", &width, &height, &nChannels, 0);
  if(!textureData) {
    throw std::runtime_error("Unable to load texture data from file system!");
  }
  Texture containerEdgeTexture(textureData, width, height, GL_RGBA);
  stbi_image_free(textureData);

  std::cout << profiler.Stop() << std::endl;

  camera.Target(0.0f, 0.0f, 0.0f);
  camera.Up(0.0f, 1.0f, 0.0f);

  objectProgram.Activate();
  objectProgram.SetConstant("material.diffuse", static_cast<int>(CONTAINER_UNIT));
  objectProgram.SetConstant("material.specular", static_cast<int>(CONTAINER_EDGE_UNIT));

  if(doOrtho) { 
    constexpr float orthoHalfHeight {ORTHO_HEIGHT*0.5f};
    constexpr float orthoHalfWidth  {orthoHalfHeight*ASPECT};
    camera.SetOrthogonal(-orthoHalfWidth, orthoHalfWidth, -orthoHalfHeight, orthoHalfHeight, NEAR, FAR);
  } else {
    camera.SetPerspective(glm::radians(45.0f), ASPECT, NEAR, FAR);
  }

  constexpr glm::vec3 lightDirection(0.0f, -1.0f, 0.0f);
  constexpr glm::vec3 lightAmbient(0.1f, 0.1f, 0.1f);
  constexpr glm::vec3 lightDiffuse(0.5f, 0.5f, 0.5f);
  constexpr glm::vec3 lightSpecular(1.0f, 1.0f, 1.0f);
  constexpr glm::vec3 pointLightPos(-0.5f, 1.0f, 0.0f);
  constexpr float pointConstant  {1.0f};
  constexpr float pointLinear    {0.22f};
  constexpr float pointQuadratic {0.2f};

  // Set directional light
  DirectionalLight directionalLight(&objectProgram, "directionalLight", lightDirection);
  directionalLight.SetAttributes({lightAmbient, lightDiffuse, lightSpecular});
  directionalLight.Apply({
    .Ambient   = "directionalLight.attributes.ambient",
    .Diffuse   = "directionalLight.attributes.diffuse",
    .Specular  = "directionalLight.attributes.specular",
    .Direction = "directionalLight.direction"
  });

  // Setup a point light (move next time)
  PointLight pointLight(&objectProgram, "pointLight", pointLightPos);
  pointLight.SetAttributes({lightAmbient, lightDiffuse, lightSpecular});
  pointLight.SetAttenuation(pointConstant, pointLinear, pointQuadratic);
  pointLight.Apply({
    .Ambient       = "pointLight.attributes.ambient",
    .Diffuse       = "pointLight.attributes.diffuse",
    .Specular      = "pointLight.attributes.specular",
    .Position      = "pointLight.position",
    .Att_Constant  = "pointLight.constant",
    .Att_Linear    = "pointLight.linear",
    .Att_Quadratic = "pointLight.quadratic",
  });
  

  const glm::mat4 projection { camera.Projection() };
  objectProgram.SetConstant("projection", projection);

  while(!glfwWindowShouldClose(pWindow)) {
    g_FrameStat.update(glfwGetTime());
    
    handleInput(pWindow);
    glfwPollEvents();

    glClearColor(0.2f, 0.3f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
    const glm::mat4 lookAt { camera.ApplyView() };
   
    objectProgram.SetConstant("cameraPosition", camera.Position());
    // Setup Cube 
    glm::mat4 modelMtx {glm::mat4(1.0f)};
    modelMtx = glm::translate(modelMtx, glm::vec3(0.0f, 0.0f, 0.0f));
    modelMtx = glm::rotate(modelMtx, static_cast<float>(glfwGetTime()), glm::vec3(1.0f, 1.0f, 1.0f));
    modelMtx = glm::scale(modelMtx, glm::vec3(0.25f, 0.25f, 0.25f));
    const glm::mat3 inverseTransposeMtx { glm::transpose(glm::inverse(modelMtx)) };


    // Setup matrix constants
    objectProgram.SetConstant("model", modelMtx);
    objectProgram.SetConstant("view", lookAt);
    objectProgram.SetConstant("inverseTransposeMtx", inverseTransposeMtx);

    // Material stuff
    objectProgram.SetConstant("material.shine", 64.0f);
    objectProgram.Activate();
    
    // Draw Cube 
    glBindVertexArray(cubeVAO);
    containerTexture.Bind(CONTAINER_UNIT);
    containerEdgeTexture.Bind(CONTAINER_EDGE_UNIT);
    glDrawElements(GL_TRIANGLES, cubeIndices.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);

    glfwSwapBuffers(pWindow);
  }
  
  glDeleteVertexArrays(1, &cubeVAO);
  glDeleteBuffers(1, &cubeVBO);
  glDeleteBuffers(1, &cubeEBO);
  
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
