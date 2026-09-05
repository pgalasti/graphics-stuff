#include "OpenGLModel.h"
#include "AssimpModelLoader.h"

#include <stb_image.h>

#include <iostream>
#include <utility>

using namespace GStuff::OpenGL;

namespace {

GLenum FormatForChannels(int channels) {
  switch(channels) {
    case 1:  return GL_RED;
    case 3:  return GL_RGB;
    case 4:  return GL_RGBA;
    default: return 0;
  }
}

}

Vertex3DNUVfModel::Vertex3DNUVfModel(const std::string& modelPath)
  : Base(modelPath) {

  ModelLoad::LoadedModel loaded {ModelLoad::Load(modelPath)};
  m_Meshes = std::move(loaded.Meshes);

  for(auto i{0uz}; i < m_Meshes.size(); ++i) {
    const auto& material {loaded.Materials[i]};

    if(Texture* pDiffuse {LoadTexture(material.DiffusePath, DIFFUSE_UNIT, "material.diffuse")}) {
      m_Meshes[i].SetTexture(pDiffuse, DIFFUSE_UNIT);
    }
    if(Texture* pSpecular {LoadTexture(material.SpecularPath, SPECULAR_UNIT, "material.specular")}) {
      m_Meshes[i].SetTexture(pSpecular, SPECULAR_UNIT);
    }
  }
}

Texture* Vertex3DNUVfModel::LoadTexture(const std::string& texturePath, GLuint unit, const std::string& samplerName) {

  if(texturePath.empty()) {
    return nullptr;
  }

  if(const auto it {m_TextureCache.find(texturePath)}; it != m_TextureCache.end()) {
    it->second->SetName(unit, samplerName);
    return it->second.get();
  }

  int width{}, height{}, channels{};
  stbi_set_flip_vertically_on_load(true);
  TextureData* pTextureData {stbi_load(texturePath.c_str(), &width, &height, &channels, 0)};
  if(!pTextureData) {
    std::cerr << "Unable to load texture '" << texturePath << "': " << stbi_failure_reason() << std::endl;
    return nullptr;
  }

  const GLenum format {FormatForChannels(channels)};
  if(format == 0) {
    std::cerr << "Unsupported channel count " << channels << " in texture '" << texturePath << "'" << std::endl;
    stbi_image_free(pTextureData);
    return nullptr;
  }

  auto pTexture {std::make_unique<Texture>(pTextureData, width, height, format)};
  stbi_image_free(pTextureData);

  pTexture->SetName(unit, samplerName);

  Texture* pRawTexture {pTexture.get()};
  m_TextureCache.emplace(texturePath, std::move(pTexture));

  return pRawTexture;
}

void Vertex3DNUVfModel::SetTexture(GStuff::General::BaseTexture<GLuint>* pTexture, GLuint unit) {
  for(auto& mesh : m_Meshes) {
    mesh.SetTexture(pTexture, unit);
  }
}

void Vertex3DNUVfModel::Draw(Program* pProgram) {
  for(auto& mesh : m_Meshes) {
    mesh.Draw(pProgram);
  } 
}
