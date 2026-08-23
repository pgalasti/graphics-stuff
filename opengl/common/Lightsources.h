#ifndef G_STUFF_LIGHTSOURCE_H
#define G_STUFF_LIGHTSOURCE_H

#include "OpenGLShaders.h"

#include <unordered_map>
#include <string>
#include <list>
#include <utility>

namespace GStuff::OpenGL {

// Maybe I can make this a derived class of something more generic in the future
class LightSources {
public:

  struct LightSourceOptions {
    std::string ActiveLightsPropName;
    // Add others later. Maybe light type names
  };

  LightSources(OpenGLProgram* pProgram, const LightSourceOptions& = {"activeLights"});
  ~LightSources() = default;	
  LightSources(LightSources&& other);
  LightSources& operator=(LightSources&& other);

  LightSources& operator=(const LightSources&) = delete;
  LightSources(const LightSources&) = delete;
  LightSources() = delete;

  using LightNumber = unsigned int;

  void SetCameraPosition(const std::string& name, const glm::vec3& position);
  void SetProperty(const std::string& constantName, const glm::vec3& property, const LightNumber num = 0);
  void Apply();
 
  // Add functions to remove light sources in future  

private:

  using LightPropertyMapEntry = std::unordered_map<std::string, glm::vec3>;
  using LightPropertyMap = std::unordered_map<LightNumber, LightPropertyMapEntry>;

  OpenGLProgram* m_pProgram;
  LightPropertyMap m_LightSourceMap;
  LightSourceOptions m_Options;

  std::pair<std::string, glm::vec3> m_CurrentCamera;

  ssize_t m_LastActiveLightCount{0};

  static constexpr ssize_t MAX_LIGHT_SOURCES { 16 };
};

}

#endif
