#include "OpenGLMesh.h"

using namespace GStuff::OpenGL;

void OpenGLMesh3DNUVf::Setup() {

  glGenVertexArrays(1, &m_VAO);

  glGenBuffers(1, &m_VBO);
  glGenBuffers(1, &m_EBO);

  glBindVertexArray(m_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

  glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(Vertex3DNUVf), m_Vertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), m_Indices.data(), GL_STATIC_DRAW);

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


void OpenGLMesh3Df::Setup() {

  glGenVertexArrays(1, &m_VAO);

  glGenBuffers(1, &m_VBO);
  glGenBuffers(1, &m_EBO);

  glBindVertexArray(m_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

  glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(Vertex3Df), m_Vertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), m_Indices.data(), GL_STATIC_DRAW);

  // Coords
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3Df), static_cast<void*>(0));
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);
}
