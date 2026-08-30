#ifndef GSTUFF_BASETEXTURE_H
#define GSTUFF_BASETEXTURE_H

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

  IDType GetID() const { return m_ID; }
protected:
  IDType m_ID{};
  int m_Width;
  int m_Height; 
};

}

#endif
