#ifndef GSTUFF_ASSIMP_MODEL_LOADER_H
#define GSTUFF_ASSIMP_MODEL_LOADER_H

#include "general/Math.h"

#include "OpenGLMesh.h"

#include <utility>
#include <algorithm>
#include <filesystem>
#include <vector>
#include <string_view>
#include <string>
#include <sstream>
#include <stdexcept>
#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace GStuff::OpenGL::ModelLoad {

using namespace GStuff::OpenGL;

const aiScene* ReadScene(Assimp::Importer& importer, std::string_view filePath);

	
using namespace GStuff::General::Math;


inline const aiScene* ReadScene(Assimp::Importer& importer, std::string_view filePath) {

  constexpr unsigned int flags = 
    (aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices);
  const aiScene* pScene { importer.ReadFile(std::string(filePath), flags) }; 

  if(!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode) {
    std::ostringstream ss;
    ss << "Error importing filePath " << filePath << " for Assimp::Importer::ReadFile!" << std::endl;
    std::cerr << ss.str();
    std::cerr << importer.GetErrorString() << std::endl;
    throw std::runtime_error("Unable to load model via Assimp importer!");
  }

  return pScene;
}

// Should probably move this
struct MeshMaterial {
  std::string DiffusePath;  // empty when the material declares none
  std::string SpecularPath;
};

struct LoadedModel {
  std::vector<OpenGLMesh3DNUVf> Meshes;
  std::vector<MeshMaterial> Materials;
};

inline std::string ResolveTexturePath(const std::filesystem::path& baseDirectory, const aiString& texturePath) {
  std::string relative {texturePath.C_Str()};
  // In case I ever work on windows
  std::replace(relative.begin(), relative.end(), '\\', '/');

  const std::filesystem::path resolved {relative};
  return resolved.is_absolute() ? resolved.string() : (baseDirectory / resolved).string();
}

inline std::string FirstTextureOfType(const aiMaterial* pMaterial, aiTextureType type,
                                      const std::filesystem::path& baseDirectory) {
  if(pMaterial->GetTextureCount(type) == 0) {
    return {};
  }

  aiString texturePath;
  if(pMaterial->GetTexture(type, 0, &texturePath) != AI_SUCCESS) {
    return {};
  }

  return ResolveTexturePath(baseDirectory, texturePath);
}

inline MeshMaterial extractMaterial(const aiMesh* pMesh, const aiScene* pScene,
                                    const std::filesystem::path& baseDirectory) {
  if(pMesh->mMaterialIndex >= pScene->mNumMaterials) {
    return {};
  }

  const aiMaterial* pMaterial {pScene->mMaterials[pMesh->mMaterialIndex]};
  return {
    .DiffusePath  = FirstTextureOfType(pMaterial, aiTextureType_DIFFUSE, baseDirectory),
    .SpecularPath = FirstTextureOfType(pMaterial, aiTextureType_SPECULAR, baseDirectory)
  };
}

inline OpenGLMesh3DNUVf extractMesh(aiMesh* pMesh) {

  std::vector<Vertex3DNUVf> vertices;
  std::vector<unsigned int> indices;

  const auto* assimpNormals {pMesh->mNormals};
  const auto* assimpTexCoords {pMesh->mTextureCoords[0]};

  for(auto i{0uz}; i < pMesh->mNumVertices; ++i) {
    const auto& assimpVertex {pMesh->mVertices[i]};
   
    // Could inline to the aggregate with emplace_back but this is fine for now 
    Vertex3DNUVf vertex;
    vertex.x = assimpVertex.x;
    vertex.y = assimpVertex.y;
    vertex.z = assimpVertex.z;
    if(assimpNormals) {
      vertex.nX = assimpNormals[i].x;
      vertex.nY = assimpNormals[i].y;
      vertex.nZ = assimpNormals[i].z;
    }
    if(assimpTexCoords) {
      vertex.u = assimpTexCoords[i].x;
      vertex.v = assimpTexCoords[i].y;
    }
    vertices.push_back(vertex);
  }

  for(unsigned int i{0}; i < pMesh->mNumFaces; ++i) {
    const auto& face {pMesh->mFaces[i]};
    for(unsigned int j{0}; j < face.mNumIndices; ++j) {
      indices.push_back(face.mIndices[j]);
    }
  }

  return OpenGLMesh3DNUVf(vertices, indices);
}

inline void ProcessAssimpNode(aiNode* pNode, const aiScene* pScene,
                              const std::filesystem::path& baseDirectory, LoadedModel& model) {

  const auto numMeshes{pNode->mNumMeshes};
  for(auto i {0u}; i < numMeshes; ++i) {
    auto mesh {pNode->mMeshes[i]};
    aiMesh* pAssimpMesh = pScene->mMeshes[mesh];
    model.Meshes.push_back(extractMesh(pAssimpMesh));
    model.Materials.push_back(extractMaterial(pAssimpMesh, pScene, baseDirectory));
  }
  
  const auto numChildren {pNode->mNumChildren};
  for(auto i{0u}; i < numChildren; ++i) {
    auto child {pNode->mChildren[i]};
    ProcessAssimpNode(child, pScene, baseDirectory, model);
  }
}

inline LoadedModel Load(std::string_view filePath) {
  Assimp::Importer importer;
  const aiScene* pScene {ReadScene(importer, filePath)};

  const std::filesystem::path baseDirectory {std::filesystem::path(filePath).parent_path()};

  LoadedModel model;
  ProcessAssimpNode(pScene->mRootNode, pScene, baseDirectory, model);

  return model;
}

}


#endif
