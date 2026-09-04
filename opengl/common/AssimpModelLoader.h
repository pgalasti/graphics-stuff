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

Vertex3DNUVfData Load(std::string_view filePath) {

  // Research assimp loading
  std::vector<Vertex3DNUVf> vertices;
  std::vector<unsigned int> indices;

  return {vertices, indices};
}


// Assimp specific functions
const aiScene* ReadScene(std::string_view filePath) {
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

void ProcessAssimpNode(aiNode* pNode, const aiScene* pScene, std::vector<OpenGLMesh3DNUVf>& meshes) {

  const auto numMeshes{pNode->mNumMeshes};
  for(auto i {0u}; i < numMeshes; ++i) {
    auto mesh {pNode->mMeshes[i]};
    aiMesh* pAssimpMesh = pScene->mMeshes[mesh];
    // meshes.push_back( Write Some Proc function ); 
  }
  
  const auto numChildren {pNode->mNumChildren};
  for(auto i{0u}; i < numChildren; ++i) {
    auto child {pNode->mChildren[i]};
    ProcessAssimpNode(child, pScene, meshes);
  }


}

}


#endif
