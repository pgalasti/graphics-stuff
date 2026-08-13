#ifndef G_STUFF_TEXTURE_H
#define G_STUFF_TEXTURE_H

#include "defines.h"

#include <GL/glew.h>

namespace GStuff::OpenGL {

// Maybe have a more general Texture class this can derive off of in future for vulkan/metal
class Texture {
public:
  Texture(const TextureData* data, int width, int height, GLenum format);
  ~Texture();

  Texture(Texture&&) = delete;
  Texture& operator=(Texture&&) = delete;
  Texture(const Texture&) = delete;
  Texture& operator=(const Texture&) = delete; 

  void Bind(GLuint num);

  GLuint GetID() const { return m_ID; }

private:
  GLuint m_ID{};
};

}
#endif
