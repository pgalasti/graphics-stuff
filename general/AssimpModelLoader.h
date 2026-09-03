#ifndef GSTUFF_ASSIMP_MODEL_LOADER_H
#define GSTUFF_ASSIMP_MODEL_LOADER_H

#include "Math.h"

#include <utility>
#include <vector>
#include <string_view>
#include <stdexcept>
#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace GStuff::General::ModelLoad {

const* aiScene ReadScene(std::string_view filePath);

	
using namespace GStuff::General::Math;

using Vertex3DNUVfData = std::pair<std::vector<Vertex3DNUVf>, std::vector<unsigned int>>;

Vertex3DNUVfData Load(std::string_view filePath) {

  // Research assimp loading
  std::vector<Vertex3DNUVf> vertices;
  std::vector<unsigned int> indices;

  return {vertices, indices};
}


// Assimp specific functions
const* aiScene ReadScene(std::string_view filePath) {
  Assimp::Importer importer;

  constexpr unsigned int flags = 
    (ai_Process_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);
  const aiScene* pScene { importer.ReadFile(filePath, flags) }; 

  if(!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode) {
    std::cerr << "Error importing filePath " + filePath + " for Assip::Importer::ReadFile!" << std::endl;
    std::cerr << importer.GetErrorString() << std::endl;
    throw std::runtime_error("Unable to load model via Assip importer!");
  }

  return pScene;
}

}


#endif
