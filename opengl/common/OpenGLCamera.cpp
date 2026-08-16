#include "OpenGLCamera.h"

#include <stdexcept>

OpenGLCamera::OpenGLCamera(float initPosX, float initPosY, float initPosZ)
  : Camera(initPosX, initPosY, initPosZ) {
}

glm::mat4 OpenGLCamera::Apply() {
  if(!m_HasTarget) {
    throw std::runtime_error("Camera is in an invalid state (Target not specified)");
  }

  m_CameraDirection = glm::normalize(m_Position - m_Target);
  m_CameraRight     = glm::normalize(glm::cross(m_Up, m_CameraDirection));
  m_CameraUp        = glm::normalize(glm::cross(m_CameraDirection, m_CameraRight));
  m_LookAt          = glm::lookAt(m_Position, m_Target, m_Up);

  return m_LookAt;
}
