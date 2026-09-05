#ifndef GSTUFF_ASSIMP_MODEL_LOADER_H
#define GSTUFF_ASSIMP_MODEL_LOADER_H

#include "general/Math.h"

#include "OpenGLMesh.h"

#include <utility>
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

const aiScene* ReadScene(std::string_view filePath);

	
using namespace GStuff::General::Math;

using Vertex3DNUVfData = std::pair<std::vector<Vertex3DNUVf>, std::vector<unsigned int>>;

inline Vertex3DNUVfData Load(std::string_view filePath) {

  // Research assimp loading
  std::vector<Vertex3DNUVf> vertices;
  std::vector<unsigned int> indices;

  return {vertices, indices};
}


// Assimp specific functions
inline const aiScene* ReadScene(std::string_view filePath) {
  Assimp::Importer importer;

  constexpr unsigned int flags = 
    (aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);
  const aiScene* pScene { importer.ReadFile(std::string(filePath), flags) }; 

  if(!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode) {
    std::stringstream ss("Error importing filePath ");
    ss << filePath << " for Assip::Importer::ReadFile!" << std::endl;
    std::cerr << ss.str();
    std::cerr << importer.GetErrorString() << std::endl;
    throw std::runtime_error("Unable to load model via Assip importer!");
  }

  return pScene;
}

inline OpenGLMesh3DNUVf extractMesh(aiMesh* pMesh, const aiScene* pScene) {

  std::vector<Vertex3DNUVf> vertices;
  std::vector<unsigned int> indices;

  for(auto i{0}; i < pMesh->mNumVertices; ++i) {
    const auto& assimpVertex {pMesh->mVertices[i]};
    const auto& assimpNormal {pMesh->mNormals[i]};
    const auto& assimpTexCoord {pMesh->mTextureCoords[0][i]};
   
    // Could inline to the aggregate with emplace_back but this is fine for now 
    Vertex3DNUVf vertex;
    vertex.x = assimpVertex.x;
    vertex.y = assimpVertex.y;
    vertex.z = assimpVertex.z;
    vertex.nX = assimpNormal.x;
    vertex.nY = assimpNormal.y;
    vertex.nZ = assimpNormal.z;
    vertex.u = assimpTexCoord.x;
    vertex.v = assimpTexCoord.y;
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

inline void ProcessAssimpNode(aiNode* pNode, const aiScene* pScene, std::vector<OpenGLMesh3DNUVf>& meshes) {

  const auto numMeshes{pNode->mNumMeshes};
  for(auto i {0u}; i < numMeshes; ++i) {
    auto mesh {pNode->mMeshes[i]};
    aiMesh* pAssimpMesh = pScene->mMeshes[mesh];
    meshes.push_back(extractMesh(pAssimpMesh, pScene));
  }
  
  const auto numChildren {pNode->mNumChildren};
  for(auto i{0u}; i < numChildren; ++i) {
    auto child {pNode->mChildren[i]};
    ProcessAssimpNode(child, pScene, meshes);
  }
}

}


#endif
