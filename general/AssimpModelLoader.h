#ifndef GSTUFF_ASSIMP_MODEL_LOADER_H
#define GSTUFF_ASSIMP_MODEL_LOADER_H

#include "Math.h"

#include <utility>
#include <vector>
#include <string_view>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace GStuff::General::ModelLoad {

using namespace GStuff::General::Math;

using Vertex3DNUVfData = std::pair<std::vector<Vertex3DNUVf>, std::vector<unsigned int>>;

Vertex3DNUVfData Load(std::string_view filePath) {

  // Research assimp loading
  std::vector<Vertex3DNUVf> vertices;
  std::vector<unsigned int> indices;

  return {vertices, indices};
}

}


#endif
