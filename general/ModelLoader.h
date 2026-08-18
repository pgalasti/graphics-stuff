#ifndef G_STUFF_MODEL_LOADER_H
#define G_STUFF_MODEL_LOADER_H

#include "Vertex.h"

#include <vector>
#include <string_view>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>
#include <unordered_map>
#include <cstdint>

namespace GStuff::General {

template <typename VertexType>
concept HasUVMembers = requires(VertexType vertex) {
  vertex.u;
  vertex.v;
};


template <typename VertexType>
class VertexLoader {
public:

  using VertexData = std::vector<VertexType>;
  using IndexData = std::vector<unsigned int>; // probably should typedef unsigned int
  using ModelData = std::pair<VertexData, IndexData>;
  using LoadOptions = unsigned long;

  enum class Format : unsigned int {
    XY = 0,
    XYUV = 1,
    XYRGB = 2,
    XYUVRGB = 3,
    XYZ = 4,
    XYZUV = 5,
    XYZRGB = 6,
    XYZUVRGB = 7
  };

  static constexpr LoadOptions LOAD_INDICES { 0x0001 };
  static constexpr LoadOptions LOAD_UV      { 0x0002 };

  VertexLoader(const Format vertex_format_input) : m_Format{vertex_format_input} {};
  virtual ~VertexLoader() = default;

  virtual ModelData Load(std::string_view path, LoadOptions loadOptions = 0) = 0;
protected:
  Format m_Format;
  
  // Kind of ugly. Will need to update these per type but will do for now
  bool IsCoordinateType() const {
    return m_Format >= Format::XY && m_Format <= Format::XYZUVRGB;
  }
  bool IsCoordinates3D() const {
    return m_Format >= Format::XYZ && m_Format <= Format::XYZUVRGB;
  }
  // The Format values encode attributes bitwise: 0x1 = UV, 0x2 = RGB, 0x4 = Z
  bool HasTextureCoordinates() const {
    return (static_cast<unsigned int>(m_Format) & 0x1u) != 0u;
  }
};

template <typename VertexType>
class WavefrontVertexLoader : public VertexLoader<VertexType> {
public:

  using Base = VertexLoader<VertexType>;
  using typename Base::Format;
  using typename Base::VertexData;
  using typename Base::IndexData;
  using typename Base::ModelData;
  using typename Base::LoadOptions;

  WavefrontVertexLoader(const Format vertex_format_input) : Base(vertex_format_input) {}
  virtual ~WavefrontVertexLoader() = default;

  // I need to come back and really refactor this so it's not so ugly when I load the rest of the attributes
  ModelData Load(std::string_view path, LoadOptions loadOptions = 0) override {
    std::ifstream file {Open(path)};

    VertexData vertexData;
    TextureCoordData textureCoords;
    CornerData corners;

    const bool wantTextureCoords {(loadOptions & Base::LOAD_UV) != 0uL};
    if(wantTextureCoords && !this->HasTextureCoordinates()) {
      std::cerr << "LOAD_UV requested but the vertex format declares no texture coordinates" << std::endl;
    }

    // Faces have to be read to pair positions with their texture coordinates, even when
    // the caller did not ask for an index buffer
    const bool readFaces {wantTextureCoords || (loadOptions & Base::LOAD_INDICES)};

    std::string line;
    while(std::getline(file, line)) {
      if(line.size() == 0uz) {
        continue;
      }

      const auto tokens {ReadLine(line)};
      if(tokens.empty()) {
        continue;
      }

      if(tokens.front() == VERTEX_TOKEN) {
        const auto required {this->IsCoordinates3D() ? 4uz : 3uz};
        if(tokens.size() < required) {
          std::cerr << "Malformed vertex line, skipping: " << line << std::endl;
          continue;
        }
        ProcessVertexLine(vertexData, tokens);
	continue;
      } else if(wantTextureCoords && tokens.front() == VERTEX_TEXTURE_COORD_TOKEN) {
        if(tokens.size() < 2uz) {
          std::cerr << "Malformed texture coordinate line, skipping: " << line << std::endl;
          continue;
        }
        ProcessTextureCoordLine(textureCoords, tokens);
        continue;
      } else if(readFaces && tokens.front() == FACE_TOKEN) {
        ProcessFaceLine(corners, tokens, vertexData.size(), textureCoords.size());
        continue;
      }

    }

    if(!wantTextureCoords) {
      IndexData indices;
      if(loadOptions & Base::LOAD_INDICES) {
        indices.reserve(corners.size());
        for(const auto& corner : corners) {
          indices.push_back(corner.Position);
        }
      }
      return {vertexData, indices};
    }

    if(textureCoords.empty()) {
      std::cerr << "Format requests texture coordinates but the file declares none" << std::endl;
    }

    return BuildTexturedModel(vertexData, textureCoords, corners, loadOptions);
  }

private:

  struct FaceCorner {
    unsigned int Position     {};
    unsigned int TextureCoord {};
    bool HasTextureCoord      {false};
  };

  struct UVCoord {
    float u {};
    float v {};
  };

  using TextureCoordData = std::vector<UVCoord>;
  using CornerData       = std::vector<FaceCorner>;

  static constexpr std::string_view COMMENT_TOKEN                {"#"};
  static constexpr std::string_view VERTEX_TOKEN                 {"v"};
  static constexpr std::string_view VERTEX_NORMAL_TOKEN          {"vn"};
  static constexpr std::string_view VERTEX_TEXTURE_COORD_TOKEN   {"vt"};
  static constexpr std::string_view MATERIAL_LIB_TOKEN           {"mtllib"};
  static constexpr std::string_view OBJECT_NAME_TOKEN            {"o"};
  static constexpr std::string_view SMOOTHE_GROUP_TOKEN          {"s"};
  static constexpr std::string_view FACE_TOKEN                   {"f"};

  std::ifstream Open(std::string_view path) const {
    std::ifstream file {std::string(path)};
    if(!file.is_open()) {
      std::cerr << "Failure loading wavefront file: " << path << std::endl;
      throw std::ios_base::failure("Failed to open wavefront file");
    }
    return file;
  }

  std::vector<std::string> ReadLine(const std::string& line) const {
    std::vector<std::string> tokens;
    std::istringstream stream(line);

    std::string token;
    while(stream >> token) {
      tokens.push_back(token);
    }

    return tokens;
  }

  void ProcessVertexLine(VertexData& vertexData, const std::vector<std::string>& tokens) {

    VertexType vertex;
    if(this->IsCoordinateType()) {
      vertex.x = std::stof(tokens[1]);
      vertex.y = std::stof(tokens[2]);
      if(this->IsCoordinates3D()) vertex.z = std::stof(tokens[3]);
    }
    vertexData.push_back(vertex);
  }

  void ProcessTextureCoordLine(TextureCoordData& textureCoords, const std::vector<std::string>& tokens) const {
    const float u {std::stof(tokens[1])};
    const float v {tokens.size() > 2uz ? std::stof(tokens[2]) : 0.0f};
    textureCoords.push_back({u, v});
  }

  bool ParseIndexField(std::string_view field, std::size_t declared, unsigned int& result) const {
    if(field.empty()) {
      return false;
    }

    const long value {std::stol(std::string(field))};
    if(value == 0L) {
      return false;
    }

    const long resolved {value > 0L ? value - 1L : static_cast<long>(declared) + value};
    if(resolved < 0L) {
      return false;
    }

    result = static_cast<unsigned int>(resolved);
    return true;
  }

  bool ParseCorner(const std::string& corner, std::size_t vertexCount, std::size_t textureCoordCount, FaceCorner& result) const {
    const auto firstSlash {corner.find('/')};
    if(!ParseIndexField(std::string_view(corner).substr(0uz, firstSlash), vertexCount, result.Position)) {
      return false;
    }

    if(firstSlash == std::string::npos) {
      return true;
    }

    const auto secondSlash {corner.find('/', firstSlash + 1uz)};
    const auto length {secondSlash == std::string::npos ? std::string::npos : secondSlash - firstSlash - 1uz};
    const auto field {std::string_view(corner).substr(firstSlash + 1uz, length)};

    result.HasTextureCoord = ParseIndexField(field, textureCoordCount, result.TextureCoord);
    return true;
  }

  void ProcessFaceLine(CornerData& corners, const std::vector<std::string>& tokens, std::size_t vertexCount, std::size_t textureCoordCount) const {
    CornerData faceCorners;
    faceCorners.reserve(tokens.size() - 1uz);
    for(auto it {tokens.begin() + 1}; it != tokens.end(); ++it) {
      FaceCorner corner;
      if(!ParseCorner(*it, vertexCount, textureCoordCount, corner)) {
        std::cerr << "Malformed face corner, skipping face: " << *it << std::endl;
        return;
      }
      faceCorners.push_back(corner);
    }

    if(faceCorners.size() < 3uz) {
      std::cerr << "Face with fewer than 3 corners, skipping" << std::endl;
      return;
    }

    for(auto i {1uz}; i + 1uz < faceCorners.size(); ++i) {
      corners.push_back(faceCorners[0uz]);
      corners.push_back(faceCorners[i]);
      corners.push_back(faceCorners[i + 1uz]);
    }
  }

  static std::uint64_t CornerKey(const FaceCorner& corner) {
    const std::uint64_t textureCoord {corner.HasTextureCoord ? corner.TextureCoord : 0xFFFFFFFFuLL};
    return (static_cast<std::uint64_t>(corner.Position) << 32) | textureCoord;
  }

  ModelData BuildTexturedModel(const VertexData& positions, const TextureCoordData& textureCoords,
                               const CornerData& corners, LoadOptions loadOptions) const {
    VertexData vertexData;
    IndexData indices;
    vertexData.reserve(corners.size());
    indices.reserve(corners.size());

    std::unordered_map<std::uint64_t, unsigned int> emitted;

    for(auto triangle {0uz}; triangle + 2uz < corners.size(); triangle += 3uz) {
      if(!TriangleInRange(corners, triangle, positions.size(), textureCoords.size())) {
        std::cerr << "Face references an out of range vertex, skipping triangle" << std::endl;
        continue;
      }

      for(auto i {triangle}; i < triangle + 3uz; ++i) {
        const auto& corner {corners[i]};
        const auto key {CornerKey(corner)};

        if(const auto found {emitted.find(key)}; found != emitted.end()) {
          indices.push_back(found->second);
          continue;
        }

        VertexType vertex {positions[corner.Position]};
        if constexpr (HasUVMembers<VertexType>) {
          if(corner.HasTextureCoord) {
            vertex.u = textureCoords[corner.TextureCoord].u;
            vertex.v = textureCoords[corner.TextureCoord].v;
          }
        }

        const auto index {static_cast<unsigned int>(vertexData.size())};
        vertexData.push_back(vertex);
        indices.push_back(index);
        emitted.emplace(key, index);
      }
    }

    if(loadOptions & Base::LOAD_INDICES) {
      return {vertexData, indices};
    }

    VertexData expanded;
    expanded.reserve(indices.size());
    for(const auto index : indices) {
      expanded.push_back(vertexData[index]);
    }
    return {expanded, IndexData{}};
  }

  bool TriangleInRange(const CornerData& corners, std::size_t offset, std::size_t vertexCount, std::size_t textureCoordCount) const {
    for(auto i {offset}; i < offset + 3uz; ++i) {
      if(corners[i].Position >= vertexCount) {
        return false;
      }
      if(corners[i].HasTextureCoord && corners[i].TextureCoord >= textureCoordCount) {
        return false;
      }
    }
    return true;
  }

};

inline std::ostream& operator<<(std::ostream& os, const std::vector<unsigned int>& indexData) {
  for(auto index : indexData) {
    os << index << ", ";
  }
  return os;
}

template <typename VertexType>
std::ostream& operator<<(std::ostream& os, const std::vector<VertexType>& vertexData) {
  for(const auto& vertex : vertexData) {
    os << vertex.x << ", " << vertex.y << ", " << vertex.z << '\n';
  }
  return os;
}

}

#endif
