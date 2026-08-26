#include "Light.h"

using namespace GStuff::OpenGL;

void DirectionalLight::Apply(const ShaderConstants& constants) {

  const auto [ambientName, diffuseName, specularName, directionName,
    unused1, unused2, unused3, unused4] = constants; // Horrible lol. Grab each from the aggregate next change
  const auto [ambient, diffuse, specular] = m_Attributes;

  m_pProgram->SetConstant(directionName, m_Direction);
  m_pProgram->SetConstant(ambientName,   ambient);
  m_pProgram->SetConstant(diffuseName,   diffuse);
  m_pProgram->SetConstant(specularName,  specular);
}

void PointLight::Apply(const ShaderConstants& constants) {
  
  const auto [ambientName, diffuseName, specularName, unused1, 
    positionName, constantName, linearName, quadraticName] = constants;
  const auto [ambient, diffuse, specular] = m_Attributes;
 
  m_pProgram->SetConstant(positionName,  m_Position); 
  m_pProgram->SetConstant(ambientName,   ambient);
  m_pProgram->SetConstant(diffuseName,   diffuse);
  m_pProgram->SetConstant(specularName,  specular);
  m_pProgram->SetConstant(constantName,  m_Constant);
  m_pProgram->SetConstant(linearName,    m_Linear);
  m_pProgram->SetConstant(quadraticName, m_Quadratic);
  
}
