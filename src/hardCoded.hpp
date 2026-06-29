#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>

namespace skrivrom
{
inline const uint32_t SIMPLE_VERTEX_COUNT = 90;

inline glm::vec3 simpleMesh[] = {
    // krychle 1
    glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 1.0f),
    glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f),

    glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 1.0f),
    glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f),

    glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),

    glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f),
    glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 1.0f),

    // krychle 2
    glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(2.0f, 0.0f, 1.0f),
    glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(2.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 1.0f),

    glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(2.0f, 1.0f, 1.0f),
    glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(2.0f, 1.0f, 1.0f), glm::vec3(2.0f, 1.0f, 0.0f),

    glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(2.0f, 1.0f, 0.0f),
    glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(2.0f, 1.0f, 0.0f), glm::vec3(2.0f, 0.0f, 0.0f),

    glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(2.0f, 0.0f, 1.0f), glm::vec3(2.0f, 1.0f, 1.0f),
    glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(2.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f),

    // krychle 3
    glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(0.0f, 2.0f, 1.0f),
    glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 2.0f, 1.0f), glm::vec3(0.0f, 2.0f, 0.0f),

    glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 2.0f, 0.0f), glm::vec3(1.0f, 2.0f, 1.0f),
    glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 2.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f),

    glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.0f, 2.0f, 1.0f), glm::vec3(1.0f, 2.0f, 1.0f),
    glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(1.0f, 2.0f, 1.0f), glm::vec3(1.0f, 2.0f, 0.0f),

    glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(1.0f, 2.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 2.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f),

    glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 2.0f, 1.0f),
    glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(1.0f, 2.0f, 1.0f), glm::vec3(0.0f, 2.0f, 1.0f),

    // jehlan
    glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(3.0f, 0.5f, 0.5f), glm::vec3(2.0f, 0.0f, 1.0f),

    glm::vec3(2.0f, 1.0f, 1.0f), glm::vec3(3.0f, 0.5f, 0.5f), glm::vec3(2.0f, 1.0f, 0.0f),

    glm::vec3(2.0f, 1.0f, 0.0f), glm::vec3(3.0f, 0.5f, 0.5f), glm::vec3(2.0f, 0.0f, 0.0f),

    glm::vec3(2.0f, 0.0f, 1.0f), glm::vec3(3.0f, 0.5f, 0.5f), glm::vec3(2.0f, 1.0f, 1.0f)};

inline GLuint simpleVbo, simpleVao;

void setupSimpleMesh()
{
  glGenVertexArrays(1, &simpleVao);
  glBindVertexArray(simpleVao);

  glGenBuffers(1, &simpleVbo);
  glBindBuffer(GL_ARRAY_BUFFER, simpleVbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * SIMPLE_VERTEX_COUNT, simpleMesh,
               GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

  glBindVertexArray(0);
}

void drawHardCoded()
{
  glBindVertexArray(simpleVao);
  glDrawArrays(GL_TRIANGLES, 0, SIMPLE_VERTEX_COUNT);
  glBindVertexArray(0);
}
}  // namespace skrivrom
