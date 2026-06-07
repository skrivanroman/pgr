#pragma once

#include <vector>
#include <cmath>
#include "ShaderProgram.hpp"
#include "Model.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace skrivrom
{
    // generates irregular water mesh
    class Water
    {
    public:
        glm::vec3 position;

        Water() {}

        //radius determines mesh size
        Water(const glm::vec3& startPos, float radius, const std::string& texturePath)
            : position(startPos), baseRadius(radius)
        {
            generateIrregularDisk(5, 30, 4.0f);

            textureHandle = pgr::createTexture(texturePath);
            glBindTexture(GL_TEXTURE_2D, textureHandle);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        void draw(const ShaderProgram::Location& location, float time) const
        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
            glUniformMatrix4fv(location.model, 1, GL_FALSE, glm::value_ptr(model));

            glm::mat4 textureMatrix = glm::mat4(1.0f);

            textureMatrix = glm::translate(textureMatrix, glm::vec3(time * 0.04f, time * 0.02f, 0.0f));

            textureMatrix = glm::rotate(textureMatrix, std::sin(time * 0.15f) * 0.08f, glm::vec3(0.0f, 0.0f, 1.0f));

            glUniformMatrix4fv(location.texTransform, 1, GL_FALSE, glm::value_ptr(textureMatrix));

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textureHandle);

            glBindVertexArray(vao);
            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

            glBindVertexArray(0);

            CHECK_GL_ERROR();
        }
    private:

        GLuint vao, vertexBuffer, indexBuffer;
        GLuint textureHandle;
        uint32_t indexCount;

        float baseRadius;

        void generateIrregularDisk(uint32_t rings, uint32_t sectors, float textureTiling)
        {
            std::vector<Vertex> vertices;
            std::vector<uint32_t> indices;

            Vertex center;
            center.position = glm::vec3(0.0f, 0.0f, 0.0f);
            center.normal = glm::vec3(0.0f, 1.0f, 0.0f);
            center.texCoords = glm::vec2(0.5f, 0.5f) * textureTiling;
            vertices.push_back(center);

            for (uint32_t r = 1; r <= rings; ++r)
            {
                float ringRatio = static_cast<float>(r) / rings;

                for (uint32_t s = 0; s < sectors; ++s)
                {
                    float angle = (static_cast<float>(s) / sectors) * 2.0f * glm::pi<float>();

                    float distortion = 1.0f + 0.18f * std::sin(angle * 5.0f) + 0.07f * std::cos(angle * 11.0f);
                    float currentRadius = baseRadius * ringRatio * distortion;

                    Vertex v;
                    v.position.x = currentRadius * std::cos(angle);
                    v.position.y = 0.0f;
                    v.position.z = currentRadius * std::sin(angle);
                    v.normal = glm::vec3(0.0f, 1.0f, 0.0f);

                    float u = v.position.x / (baseRadius * 2.0f) + 0.5f;
                    float vCoord = v.position.z / (baseRadius * 2.0f) + 0.5f;
                    v.texCoords = glm::vec2(u, vCoord) * textureTiling;

                    vertices.push_back(v);
                }
            }

            for (uint32_t s = 0; s < sectors; ++s)
            {
                uint32_t nextS = (s + 1) % sectors;
                indices.push_back(0);
                indices.push_back(1 + nextS);
                indices.push_back(1 + s);
            }

            for (uint32_t r = 1; r < rings; ++r)
            {
                uint32_t currentRingStart = 1 + (r - 1) * sectors;
                uint32_t nextRingStart = 1 + r * sectors;

                for (uint32_t s = 0; s < sectors; ++s)
                {
                    uint32_t nextS = (s + 1) % sectors;

                    uint32_t currRingVert = currentRingStart + s;
                    uint32_t currRingNextVert = currentRingStart + nextS;
                    uint32_t nextRingVert = nextRingStart + s;
                    uint32_t nextRingNextVert = nextRingStart + nextS;

                    indices.push_back(currRingVert);
                    indices.push_back(currRingNextVert);
                    indices.push_back(nextRingVert);

                    indices.push_back(currRingNextVert);
                    indices.push_back(nextRingNextVert);
                    indices.push_back(nextRingVert);

                }
            }

            indexCount = static_cast<uint32_t>(indices.size());

            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &vertexBuffer);
            glGenBuffers(1, &indexBuffer);

            glBindVertexArray(vao);

            glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

            glBindVertexArray(0);
        }

    };
}