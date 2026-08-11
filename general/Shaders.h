#ifndef G_SHADERS_H
#define G_SHADERS_H

#include <string>
#include <vector>
#include <memory>
#include <concepts>
#include <utility>

namespace GStuff::General::Shaders {

class Shader {
public:

  enum class ShaderType {
    Vertex,
    Fragment
    // Add others here later
  };

  Shader(const std::string& path, const ShaderType type) : m_Path{path}, m_Type{type} {}
  virtual ~Shader() = default;

  virtual unsigned int ShaderID() const = 0;
  virtual std::string ReadShaderText() const = 0;

  const std::string& Path() const { return m_Path; }
  ShaderType Type() const { return m_Type; }

protected:
  std::string m_Path;
  ShaderType m_Type;
};


using ShaderPtr = std::unique_ptr<Shader>;

class Program {
public:
  template <std::convertible_to<ShaderPtr>... Shaders>
  explicit Program(Shaders&&... shaders) {
    m_Shaders.reserve(sizeof...(shaders));
    (m_Shaders.push_back(std::forward<Shaders>(shaders)), ...);
  }

  virtual ~Program() = default;

  virtual void Activate(bool active = true) = 0;

protected:
  std::vector<ShaderPtr> m_Shaders;
};

}
#endif
