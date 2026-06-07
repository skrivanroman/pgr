#pragma once

#include <vector>
#include "Mesh.hpp"

namespace skrivrom
{
    // generates flat ground with two textures combined
    class Ground
    {
    public:
        Ground() {}

        // size is width and height of the square
        // resolution determines the number of quads
        // textureTiling sets repetition of the textures
        Ground(float size, uint32_t resolution, float textureTiling, const ShaderProgram::Location& location,
            const std::string& path1, const std::string& path2
        )
        {
            generate(size, resolution, textureTiling);
            setupMesh(location);
            texture1 = pgr::createTexture(path1);
            glBindTexture(GL_TEXTURE_2D, texture1);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            texture2 = pgr::createTexture(path2);
            glBindTexture(GL_TEXTURE_2D, texture2);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            glBindTexture(GL_TEXTURE_2D, 0);

            material.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
            material.ambient = glm::vec3(0.35f, 0.35f, 0.35f);
            material.specular = glm::vec3(0.08f, 0.08f, 0.08f);
            material.shininess = 8.0f;
            material.hasTexture = true;
        }

        void draw(const ShaderProgram::Location& location) const
        {
            glm::mat4 model = transform.calcModel();
            glUniformMatrix4fv(location.model, 1, GL_FALSE, glm::value_ptr(model));

            material.apply(location);

            glUniform1i(location.sampler, 0);
            glUniform1i(location.sampler2, 1);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture1);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, texture2);

            glBindVertexArray(vao);
            CHECK_GL_ERROR();
            glDrawElements(GL_TRIANGLES, static_cast<uint32_t>(indices.size()), GL_UNSIGNED_INT, 0);
            CHECK_GL_ERROR();

            glBindTexture(GL_TEXTURE_2D, 0);
            glBindVertexArray(0);
        }

    private:
        GLuint vao = 0, vertexBuff = 0, indexBuff = 0;
        GLuint texture1, texture2;

        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        Material material;
        Transform transform;

    private:
        void generate(float size, uint32_t resolution, float textureTiling)
        {
            float half = size * 0.5f;
            for (uint32_t z = 0; z <= resolution; ++z)
            {
                for (uint32_t x = 0; x <= resolution; ++x)
                {
                    float xf = (float)x / resolution;
                    float zf = (float)z / resolution;

                    Vertex v;

                    v.position = glm::vec3(
                        -half + xf * size,
                        0.0f,
                        -half + zf * size
                    );

                    v.normal = glm::vec3(0.0f, 1.0f, 0.0f);

                    v.texCoords = glm::vec2(
                        xf * textureTiling,
                        zf * textureTiling
                    );

                    vertices.push_back(v);
                }
            }

            for (uint32_t z = 0; z < resolution; ++z)
            {
                for (uint32_t x = 0; x < resolution; ++x)
                {
                    uint32_t topLeft = z * (resolution + 1) + x;
                    uint32_t topRight = topLeft + 1;

                    uint32_t bottomLeft = (z + 1) * (resolution + 1) + x;
                    uint32_t bottomRight = bottomLeft + 1;

                    indices.push_back(topLeft);
                    indices.push_back(bottomLeft);
                    indices.push_back(topRight);

                    indices.push_back(topRight);
                    indices.push_back(bottomLeft);
                    indices.push_back(bottomRight);
                }
            }
        }

        void setupMesh(const ShaderProgram::Location& location)
        {
            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &vertexBuff);
            glGenBuffers(1, &indexBuff);

            glBindVertexArray(vao);

            glBindBuffer(GL_ARRAY_BUFFER, vertexBuff);
            glBufferData(
                GL_ARRAY_BUFFER,
                vertices.size() * sizeof(Vertex),
                vertices.data(),
                GL_STATIC_DRAW
            );

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuff);
            glBufferData(
                GL_ELEMENT_ARRAY_BUFFER,
                indices.size() * sizeof(uint32_t),
                indices.data(),
                GL_STATIC_DRAW
            );

            glEnableVertexAttribArray(location.postion);
            glVertexAttribPointer(
                location.postion,
                3,
                GL_FLOAT,
                GL_FALSE,
                sizeof(Vertex),
                (void*)offsetof(Vertex, position)
            );

            glEnableVertexAttribArray(location.normal);
            glVertexAttribPointer(
                location.normal,
                3,
                GL_FLOAT,
                GL_FALSE,
                sizeof(Vertex),
                (void*)offsetof(Vertex, normal)
            );

            glEnableVertexAttribArray(location.texCoords);
            glVertexAttribPointer(
                location.texCoords,
                2,
                GL_FLOAT,
                GL_FALSE,
                sizeof(Vertex),
                (void*)(offsetof(Vertex, texCoords))
            );

            CHECK_GL_ERROR();
            glBindVertexArray(0);
        }
    };
}