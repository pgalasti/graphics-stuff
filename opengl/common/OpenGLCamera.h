#ifndef G_STUFF_OPENGL_CAMERA_H
#define G_STUFF_OPENGL_CAMERA_H

#include "general/Camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace GStuff::General;


class OpenGLCamera : public Camera<glm::mat4, glm::vec3, float> {
public:
  OpenGLCamera(float initPosX = 0.0f, float initPosY = 0.0f, float initPosZ = 0.0f);
  ~OpenGLCamera() override = default;

  glm::mat4 Apply() override;

private:

  // Position/target/up live in the base now

  // Keeping as members so we don't have to construct new objects per apply call
  glm::mat4 m_LookAt;
  glm::vec3 m_CameraDirection;
  glm::vec3 m_CameraUp;
  glm::vec3 m_CameraRight;
};

#endif

