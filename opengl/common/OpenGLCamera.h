#ifndef G_STUFF_OPENGL_CAMERA_H
#define G_STUFF_OPENGL_CAMERA_H

#include "general/Camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace GStuff::General;

enum class ProjectionType {
 NotSet, Orthogonal, Perspective 
};

class OpenGLCamera : public Camera<glm::mat4, glm::vec3, float> {
public:
  OpenGLCamera(float initPosX = 0.0f, float initPosY = 0.0f, float initPosZ = 0.0f);
  ~OpenGLCamera() override = default;

  void Move(Camera::Direction direction, float step) override;
  void Rotate(Camera::Rotation rotation, float angle) override;

  glm::mat4 ApplyView() override;
  glm::mat4 Projection() override;

  // Type set by last function called. Maybe a setter could be used later if needed
  void SetOrthogonal(float left, float right, float bottom, float top, float near, float far) override;
  void SetPerspective(float fovRadians, float aspectRatio, float near, float far) override;

protected:
  void OnTargetChanged() override;

private:

  void UpdateBasis();

  struct ProjectionMetrics {
    
    float Near;
    float Far;

    // Ortho specific
    float Left, Right, Bottom, Top;

    // Perspective specific
    float FovRadians, AspectRatio;

  };

  struct DirectionMetrics {
    float Pitch {};
    float Yaw   {-90.0f};
    float Roll  {};
  };

  // Keeping as members so we don't have to construct new objects per apply call
  glm::mat4 m_LookAt;
  glm::vec3 m_CameraDirection;
  glm::vec3 m_CameraUp;
  glm::vec3 m_CameraRight;

  ProjectionType m_ProjectionType {ProjectionType::NotSet};
  ProjectionMetrics m_ProjectionMetrics;
  DirectionMetrics m_DirectionMetrics;
};

#endif

