#ifndef G_STUFF_CAMERA_H
#define G_STUFF_CAMERA_H

namespace GStuff::General {

template <typename ViewType, typename VecType, typename T = float>
class Camera {
public:

 // Should have a specification for whatever viewport this is but I can do that later
 virtual ~Camera() = default;

 void Target(T x, T y, T z) { m_Target = VecType(x, y, z); m_HasTarget = true; }
 void Up(T x, T y, T z)     { m_Up = VecType(x, y, z); }
 void GoTo(T x, T y, T z)   { m_Position = VecType(x, y, z); }

 virtual ViewType Apply() = 0;

 // I can get fancy with angle later

protected:
 Camera(T initPosX = T{}, T initPosY = T{}, T initPosZ = T{})
   : m_Position(initPosX, initPosY, initPosZ) {}

 Camera(const Camera&) = default;
 Camera& operator=(const Camera&) = default;

 VecType m_Position;
 VecType m_Target     {T{0}, T{0}, T{0}};
 VecType m_Up         {T{0}, T{1}, T{0}};

 bool m_HasTarget {false}; // Kind of hacky but it's ok
};



}

#endif 
