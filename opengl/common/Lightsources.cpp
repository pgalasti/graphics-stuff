#include "Lightsources.h"

#include <format>
#include <stdexcept>

using namespace GStuff::OpenGL;

LightSources::LightSources(OpenGLProgram* pProgram) : m_pProgram{pProgram} {}

LightSources::LightSources(LightSources&& other) {
  m_pProgram = other.m_pProgram;
  other.m_pProgram = nullptr;

  m_LightSourceMap = other.m_LightSourceMap;
  other.m_LightSourceMap.clear();
}

LightSources& LightSources::operator=(LightSources&& other) {
  m_pProgram = other.m_pProgram;
  other.m_pProgram = nullptr;

  m_LightSourceMap = other.m_LightSourceMap;
  other.m_LightSourceMap.clear();

  return *this;
}

void LightSources::SetCameraPosition(const std::string& name, const glm::vec3& position) {
  m_CurrentCamera.first = name;
  m_CurrentCamera.second = position;
}

void LightSources::SetProperty(const std::string& constantName, const glm::vec3& property, const LightNumber num/*=0*/) {

  if(num > MAX_LIGHT_SOURCES) throw std::runtime_error(std::format("You can only have a max light source of {}", MAX_LIGHT_SOURCES));

  auto iter {m_LightSourceMap.find(num) };
  if(iter != m_LightSourceMap.end()) {
    iter->second[constantName] = property;
  } else {
    LightPropertyMapEntry entry;
    entry[constantName] = property;
    m_LightSourceMap[num] = entry;
  }
  
}

void LightSources::Apply() {
  m_pProgram->SetConstant(m_CurrentCamera.first, m_CurrentCamera.second);

  for(auto i {0z}; i < MAX_LIGHT_SOURCES; ++i) {
    const auto iter {m_LightSourceMap.find(i)};
    if(iter == m_LightSourceMap.end()) {
      // Bail out early to save cycles
      break;
    }

    for(const auto& entry : iter->second) {
      m_pProgram->SetConstant(entry.first, entry.second);
    }
  }  
}
