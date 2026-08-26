#include "Light.h"

using namespace GStuff::OpenGL;

void DirectionalLight::Apply(const ShaderConstants& constants) {

  const auto [ambientName, diffuseName, specularName, directionName] = constants;
  const auto [ambient, diffuse, specular] = m_Attributes;

  m_pProgram->SetConstant(directionName, m_Direction);
  m_pProgram->SetConstant(ambientName,   ambient);
  m_pProgram->SetConstant(diffuseName,   diffuse);
  m_pProgram->SetConstant(specularName,  specular);
}
