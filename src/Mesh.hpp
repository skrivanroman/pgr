#pragma once

#include "ShaderProgram.hpp"
#include <pgr.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include <vector>

struct Vertex 
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

struct Material
{
	glm::vec3 diffuse;
	glm::vec3 ambient;
	glm::vec3 specular;
	float shininess;
	GLuint texture;
	bool hasTexture;
};

class Mesh
{
public:
	Mesh() {}

	Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const Material& material, const ShaderProgram::Location& location);

	void draw(const ShaderProgram::Location& location) const;

private:

	GLuint vao, vertexBuff, indexBuff;
	uint32_t indices;
	Material material;
};