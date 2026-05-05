#pragma once

#include "ShaderProgram.hpp"

class DirLight
{
public:
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    void apply(const ShaderProgram::Location& loc) const
    {
        glUniform3fv(loc.dirLight.direction, 1, &direction[0]);
        glUniform3fv(loc.dirLight.ambient, 1, &ambient[0]);
        glUniform3fv(loc.dirLight.diffuse, 1, &diffuse[0]);
        glUniform3fv(loc.dirLight.specular, 1, &specular[0]);
    }
};

class PointLight
{
public:
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant = 1.0f;
    float linear = 0.09f;

    void apply(const ShaderProgram::Location& loc) const
    {
        glUniform3fv(loc.pointLight.position, 1, &position[0]);
        glUniform3fv(loc.pointLight.ambient, 1, &ambient[0]);
        glUniform3fv(loc.pointLight.diffuse, 1, &diffuse[0]);
        glUniform3fv(loc.pointLight.specular, 1, &specular[0]);

        glUniform1f(loc.pointLight.constant, constant);
        glUniform1f(loc.pointLight.linear, linear);
    }
};

class SpotLight
{
public:
    glm::vec3 position;
    glm::vec3 direction;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float cutOff;
    float outerCutOff;

    float constant = 1.0f;
    float linear = 0.09f;

    void apply(const ShaderProgram::Location& loc) const
    {
        glUniform3fv(loc.spotLight.position, 1, &position[0]);
        glUniform3fv(loc.spotLight.direction, 1, &direction[0]);

        glUniform3fv(loc.spotLight.ambient, 1, &ambient[0]);
        glUniform3fv(loc.spotLight.diffuse, 1, &diffuse[0]);
        glUniform3fv(loc.spotLight.specular, 1, &specular[0]);

        glUniform1f(loc.spotLight.cutOff, cutOff);
        glUniform1f(loc.spotLight.outerCutOff, outerCutOff);

        glUniform1f(loc.spotLight.constant, constant);
        glUniform1f(loc.spotLight.linear, linear);
    }
};