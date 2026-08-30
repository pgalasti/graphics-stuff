#include "Texture.h"

using namespace GStuff::OpenGL;
using namespace GStuff::General;

Texture::Texture(const TextureData* data, int width, int height, GLenum format) 
  : BaseTexture(width, height) {

  // Upload without disturbing the sampler state: whatever unit is active keeps
  // the binding it had, so Bind() is the only thing that decides unit ownership.
  GLint previousBinding {};
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousBinding);

  glGenTextures(1, &m_ID);
  glBindTexture(GL_TEXTURE_2D, m_ID);

  // Rows of GL_RGB data are only 4-byte aligned when width is a multiple of 4
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // Configure GL_REPEAT later
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  //Configure mipmaping later
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  glBindTexture(GL_TEXTURE_2D, previousBinding);
}

Texture::~Texture() {
  glDeleteTextures(1, &m_ID);
}

void Texture::Bind(GLuint num) {
  glActiveTexture(GL_TEXTURE0 + num);
  glBindTexture(GL_TEXTURE_2D, m_ID);
}
