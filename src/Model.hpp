#pragma once

#include "Mesh.hpp"

struct Texture
{
	GLuint id;
	std::string path;
};

struct Transform
{
	glm::vec3 position{ 0.0f };
	glm::vec3 rotation{ 0.0f };
	glm::vec3 scale{ 1.0f };
};

class Model
{
public:
	std::string directory;
	Transform transform;

	Model(){}

	Model(const std::string& path, const ShaderProgram::Location& location)
	{
		loadFromFile(path, location);
	}

	void draw(const ShaderProgram::Location& location) const;

private:
	void loadFromFile(const std::string& path, const ShaderProgram::Location& location);
	void processNode(const aiNode* node, const aiScene* scene, const ShaderProgram::Location& location);
	Mesh processMesh(const aiMesh* mesh, const aiScene* scene, const ShaderProgram::Location& location);

	std::vector<Mesh> meshes;
	std::vector<Texture> textures;
};