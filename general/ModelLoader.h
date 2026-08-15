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

namespace GStuff::General {


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
    IndexData indices; 

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
      } else if((loadOptions & Base::LOAD_INDICES) && tokens.front() == FACE_TOKEN) {
        ProcessFaceLine(indices, tokens);
        continue;
      }

    }
    return {vertexData, indices};
  }

private:

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

  unsigned int ParsePositionIndex(const std::string& corner) const {
    const auto field {corner.substr(0uz, corner.find('/'))};
    return static_cast<unsigned int>(std::stoul(field) - 1uL);
  }

  void ProcessFaceLine(IndexData& indices, const std::vector<std::string>& tokens) const {
    IndexData corners;
    corners.reserve(tokens.size() - 1uz);
    for(auto it {tokens.begin() + 1}; it != tokens.end(); ++it) {
      corners.push_back(ParsePositionIndex(*it));
    }

    if(corners.size() < 3uz) {
      std::cerr << "Face with fewer than 3 corners, skipping" << std::endl;
      return;
    }

    for(auto i {1uz}; i + 1uz < corners.size(); ++i) {
      indices.push_back(corners[0uz]);
      indices.push_back(corners[i]);
      indices.push_back(corners[i + 1uz]);
    }
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
