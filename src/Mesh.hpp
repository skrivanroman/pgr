#pragma once

#include "ShaderProgram.hpp"
#include <pgr.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include <vector>

namespace skrivrom
{
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
		GLuint texture = 0;
		bool hasTexture;
		float alpha = 1.0f;

		void apply(const ShaderProgram::Location& location) const
		{
			glUniform3fv(location.ambient, 1, glm::value_ptr(ambient));
			glUniform3fv(location.diffuse, 1, glm::value_ptr(diffuse));
			glUniform3fv(location.specular, 1, glm::value_ptr(specular));
			glUniform1f(location.shininess, shininess);
			glUniform1i(location.hasTexture, hasTexture);
			glUniform1f(location.alpha, alpha);
		}
	};

	// part of the model can have it's own material
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
}