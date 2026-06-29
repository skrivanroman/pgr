#pragma once

#include "Model.hpp"
#include <string>
#include "utils.hpp"

namespace skrivrom
{
    // holds two spritesheets
    // animates them with time
    class AnimatedSprite
    {
    public:

        Transform transform;
        uint32_t rows, cols;
        float animationSpeed;

        AnimatedSprite() {}

        // paths to spritesheets and rows, cols are their dimensions
        AnimatedSprite(const std::string& path1, const std::string& path2, uint32_t rows, uint32_t cols, float speed = 15.0f)
            : rows(rows), cols(cols), animationSpeed(speed)
        {
            texture1 = loadSRGBTexture(path1);
            texture2 = loadSRGBTexture(path2);

            firstActive = true;
            time = 0.0f;

            initBuffers();
        }

        void update(float deltaTime)
        {
            time += deltaTime;
            int totalFrames = rows * cols;
            currFrame = static_cast<int>(time * animationSpeed) % totalFrames;
        }

        void draw(const SpriteLocation& location)
        {
            glBindVertexArray(vao);

            glm::vec2 scale = getUVScale();
            glUniform2fv(location.uvScale, 1, glm::value_ptr(scale));
            glm::vec2 offset = getUVOffset();
            glUniform2fv(location.uvOffset, 1, glm::value_ptr(offset));

            glm::mat4 model = transform.calcModel();
            glUniformMatrix4fv(location.model, 1, GL_FALSE, glm::value_ptr(model));

            glActiveTexture(GL_TEXTURE0);
            if (firstActive)
            {
                glBindTexture(GL_TEXTURE_2D, texture1);
            }
            else
            {
                glBindTexture(GL_TEXTURE_2D, texture2);
            }

            glDrawArrays(GL_TRIANGLES, 0, 6);

            glBindVertexArray(0);
            glBindTexture(GL_TEXTURE_2D, 0);

        }

        void toggleState()
        {
            firstActive = !firstActive;
        }

        glm::vec2 getUVScale() const
        {
            return glm::vec2(1.0f / cols, 1.0f / rows);
        }

        glm::vec2 getUVOffset() const
        {
            int column = currFrame % cols;
            int row = currFrame / cols;

            return glm::vec2(column * (1.0f / cols), row * (1.0f / rows));
        }

    private:
        void initBuffers()
        {
            float vertices[] = {
                -0.5f, -0.5f, 0.0f,   0.0f, 0.0f,
                 0.5f, -0.5f, 0.0f,   1.0f, 0.0f,
                 0.5f,  0.5f, 0.0f,   1.0f, 1.0f,

                -0.5f, -0.5f, 0.0f,   0.0f, 0.0f,
                 0.5f,  0.5f, 0.0f,   1.0f, 1.0f,
                -0.5f,  0.5f, 0.0f,   0.0f, 1.0f
            };

            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &vbo);

            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }


        GLuint texture1, texture2;
        GLuint vao, vbo;
        bool firstActive;
        float time;
        int currFrame;
    };
}
