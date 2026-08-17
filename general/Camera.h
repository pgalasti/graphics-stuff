#ifndef G_STUFF_CAMERA_H
#define G_STUFF_CAMERA_H

namespace GStuff::General {

enum class CameraDirection {
  Forward, Backward, Left, Right, Up, Down
};

template <typename ViewType, typename VecType, typename T = float>
class Camera {
public:

 using Direction = CameraDirection;

 // Should have a specification for whatever viewport this is but I can do that later
 virtual ~Camera() = default;

 void Target(T x, T y, T z) { m_Target = VecType(x, y, z); m_HasTarget = true; OnTargetChanged(); }
 void Up(T x, T y, T z)     { m_Up = VecType(x, y, z); }
 void GoTo(T x, T y, T z)   { m_Position = VecType(x, y, z); }

 // Maybe I should make this a vector value insead of direction with a scalar?
 virtual void Move(Direction direction, T step) = 0;

 virtual ViewType Apply() = 0;

 // I can get fancy with angle later

protected:
 Camera(T initPosX = T{}, T initPosY = T{}, T initPosZ = T{})
   : m_Position(initPosX, initPosY, initPosZ) {}

 virtual void OnTargetChanged() {}

 Camera(const Camera&) = default;
 Camera& operator=(const Camera&) = default;

 VecType m_Position;
 VecType m_Target      {T{0}, T{0}, T{0}};
 VecType m_Up          {T{0}, T{1}, T{0}};
 VecType m_CameraFront {T{0}, T{0}, T{-1}};

 bool m_HasTarget {false}; // Kind of hacky but it's ok
};



}

#endif 
