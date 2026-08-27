#include "Light.h"

using namespace GStuff::OpenGL;

void DirectionalLight::Apply(const ShaderConstants& constants) {

  m_ShaderConstants = constants;
  
  Update();
}

void DirectionalLight::Update() {
  const std::string ambientName   { m_ShaderConstants.Ambient };
  const std::string diffuseName   { m_ShaderConstants.Diffuse };
  const std::string specularName  { m_ShaderConstants.Specular };
  const std::string directionName { m_ShaderConstants.Direction };
  const auto [ambient, diffuse, specular] = m_Attributes;

  m_pProgram->SetConstant(directionName, m_Direction);
  m_pProgram->SetConstant(ambientName,   ambient);
  m_pProgram->SetConstant(diffuseName,   diffuse);
  m_pProgram->SetConstant(specularName,  specular);

}

void PointLight::Apply(const ShaderConstants& constants) {
  
  m_ShaderConstants = constants;
  
  Update(); 
}

void PointLight::Update() {
  
  const std::string ambientName   { m_ShaderConstants.Ambient };
  const std::string diffuseName   { m_ShaderConstants.Diffuse };
  const std::string specularName  { m_ShaderConstants.Specular };
  const std::string positionName  { m_ShaderConstants.Position };
  const std::string constantName  { m_ShaderConstants.Att_Constant};
  const std::string linearName    { m_ShaderConstants.Att_Linear};
  const std::string quadraticName { m_ShaderConstants.Att_Quadratic};
  const auto [ambient, diffuse, specular] = m_Attributes;
 
  m_pProgram->SetConstant(positionName,  m_Position); 
  m_pProgram->SetConstant(ambientName,   ambient);
  m_pProgram->SetConstant(diffuseName,   diffuse);
  m_pProgram->SetConstant(specularName,  specular);
  m_pProgram->SetConstant(constantName,  m_Constant);
  m_pProgram->SetConstant(linearName,    m_Linear);
  m_pProgram->SetConstant(quadraticName, m_Quadratic);

}
