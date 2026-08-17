#include "OpenGLCamera.h"

#include <stdexcept>

OpenGLCamera::OpenGLCamera(float initPosX, float initPosY, float initPosZ)
  : Camera(initPosX, initPosY, initPosZ) {
}

void OpenGLCamera::OnTargetChanged() {
  if(m_Target == m_Position) {
    throw std::runtime_error("Camera cannot target the position it is sitting at");
  }

  m_CameraFront = glm::normalize(m_Target - m_Position);
}

void OpenGLCamera::UpdateBasis() {
  m_CameraDirection = -m_CameraFront;
  m_CameraRight     = glm::normalize(glm::cross(m_CameraFront, m_Up));
  m_CameraUp        = glm::normalize(glm::cross(m_CameraRight, m_CameraFront));
}

glm::mat4 OpenGLCamera::Apply() {
  if(!m_HasTarget) {
    throw std::runtime_error("Camera is in an invalid state (Target not specified)");
  }

  UpdateBasis();
  m_LookAt = glm::lookAt(m_Position, m_Position+m_CameraFront, m_CameraUp);

  return m_LookAt;
}

void OpenGLCamera::Move(Camera::Direction direction, float step) {
  UpdateBasis();

  switch (direction) {
  case Camera::Direction::Forward:
    m_Position += step*m_CameraFront;
    break;
  case Camera::Direction::Backward:
    m_Position -= step*m_CameraFront;
    break;
  case Camera::Direction::Left:
    m_Position -= step*m_CameraRight;
    break;
  case Camera::Direction::Right:
    m_Position += step*m_CameraRight;
    break;
  case Camera::Direction::Up:
    m_Position += step*m_Up;
    break;
  case Camera::Direction::Down:
    m_Position -= step*m_Up;
    break;
  }

}
