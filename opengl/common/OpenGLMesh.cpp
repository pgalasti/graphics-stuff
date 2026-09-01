#include "OpenGLMesh.h"

using namespace GStuff::OpenGL;

void OpenGLMesh::Draw(Program* pProgram) {
  for(auto element : m_TexturePtrs) {
    auto[pTexture, positionNum] = element;

    // Need a way to know the name of the constant being set here

    pTexture->Bind(positionNum);
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
  }
}

void OpenGLMesh::Setup() {
}

void OpenGLMesh::SetTexture(GStuff::General::BaseTexture<GLuint>* pTexture, TextureID positionNum) {
  m_TexturePtrs.push_back({pTexture, positionNum);
}

void OpenGLMesh::Init() {

  glGenVertexArrays(1, &m_VAO);

  glGenBuffers(1, &m_VBO);
  glGenBuffers(1, &m_EBO);

  glBindVertexArray(m_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

  glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(Vertex3DNUVf), &m_Vertices[0], GL_STATIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), &m_Indices[0], GL_STATIC_DRAW);

  // Coords
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3DNUVf), static_cast<void*>(0));
  glEnableVertexAttribArray(0);
  // Normal
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3DNUVf), (void*)(sizeof(float)*3));
  glEnableVertexAttribArray(1);
  // Texture coords
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3DNUVf), (void*)(sizeof(float)*6));
  glEnableVertexAttribArray(2);

  glBindVertexArray(0);
}

void OpenGLMesh::Teardown() {

  glDeleteVertexArrays(1, &m_VAO);
  glDeleteBuffers(1, &m_VBO);
  glDeleteBuffers(1, &m_EBO);
}

