#ifndef G_STUFF_TEXTURE_H
#define G_STUFF_TEXTURE_H

#include "defines.h"
#include "general/BaseTexture.h"

#include <GL/glew.h>

#include <utility>

namespace GStuff::OpenGL {

class Texture : public GStuff::General::BaseTexture<GLuint> {

public:
  Texture(const TextureData* data, int width, int height, GLenum format);
  ~Texture();

  Texture(Texture&& other) noexcept : BaseTexture<GLuint>(std::move(other)) {
    other.m_ID = 0;
  }

  Texture& operator=(Texture&& other) noexcept {
    if(this != &other) {
      glDeleteTextures(1, &m_ID);
      BaseTexture<GLuint>::operator=(std::move(other));
      other.m_ID = 0;
    }
    return *this;
  }

  Texture(const Texture&) = delete;
  Texture& operator=(const Texture&) = delete;

  void Bind(GLuint num) override;
};

}
#endif
