#ifndef GSTUFF_BASETEXTURE_H
#define GSTUFF_BASETEXTURE_H

#include <string>
#include <unordered_map>
#include <vector>

namespace GStuff::General {

template<typename IDType>
class BaseTexture {
public:
  BaseTexture(int width, int height) : m_Width{width}, m_Height{height} {}
  virtual ~BaseTexture() = default;

  BaseTexture(BaseTexture<IDType>&& other) noexcept = default;
  BaseTexture& operator=(BaseTexture<IDType>&& other) noexcept = default;

  BaseTexture(const BaseTexture<IDType>&) = delete;
  BaseTexture& operator=(const BaseTexture<IDType>&) = delete;

  virtual void Bind(IDType) = 0;

  using NameMap = std::unordered_map<IDType, std::string>;
  using NameSamplePair = std::pair<IDType, std::string>;

  void SetName(IDType id, const std::string& name) { m_NameMap[id] = name; }
  std::string GetName(IDType id) const {
    const auto it {m_NameMap.find(id)};
    return it == m_NameMap.end() ? std::string{} : it->second;
  }

  std::vector<NameSamplePair> GetConstants() const {
    std::vector<NameSamplePair> constants;
    constants.reserve(m_NameMap.size());
    for(const auto& [id, name] : m_NameMap) {
      constants.push_back({id, name});
    }
    return constants;
  }

  IDType GetID() const { return m_ID; }
protected:
  IDType m_ID{};
  int m_Width;
  int m_Height;
  NameMap m_NameMap;
};

}

#endif
