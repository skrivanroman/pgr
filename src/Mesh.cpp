#include "Mesh.hpp"

#include <iostream>

namespace skrivrom
{
Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices,
           const Material& material, const Location& location)
{
  this->indices = static_cast<uint32_t>(indices.size());
  this->material = material;

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vertexBuff);
  glGenBuffers(1, &indexBuff);

  glBindVertexArray(vao);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuff);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(),
               GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, vertexBuff);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(location.position);
  glVertexAttribPointer(location.postion, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

  glEnableVertexAttribArray(location.normal);
  glVertexAttribPointer(location.normal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void*)offsetof(Vertex, normal));

  glEnableVertexAttribArray(location.texCoords);
  glVertexAttribPointer(location.texCoords, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void*)offsetof(Vertex, texCoords));

  glBindVertexArray(0);
}

void Mesh::draw(const Location& location) const
{
  material.apply(location);

  if (material.hasTexture)
  {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, material.texture);
  }

  glBindVertexArray(vao);
  glDrawElements(GL_TRIANGLES, indices, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}
}  // namespace skrivrom
