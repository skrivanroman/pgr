#pragma once

#include "Mesh.hpp"
#include <glm/gtx/quaternion.hpp>

namespace skrivrom
{
	struct Texture
	{
		GLuint id;
		std::string path;
	};

	struct Transform
	{
		glm::vec3 position{ 0.0f };
		glm::quat rotation = glm::quat(1, 0, 0, 0);
		glm::quat importCorr = glm::quat(1, 0, 0, 0);
		glm::vec3 scale{ 1.0f };

		glm::mat4 calcModel() const
		{

			glm::mat4 tr = glm::translate(glm::mat4(1.0f), position);

			glm::mat4 rot = glm::toMat4(rotation * importCorr);

			glm::mat4 sc = glm::scale(glm::mat4(1.0f), scale);

			return tr * rot * sc;
		}
	};

	struct BoundBox
	{
		glm::vec3 minPoint;
		glm::vec3 maxPoint;
		glm::vec3 center;
		float width, height, depth;
	};

	// holds one independent 3D object
	// is tied to one shader
	class Model
	{
	public:
		std::string directory;
		Transform transform;
		BoundBox modelBounds;
		bool visible = true;

		Model() {}

		Model(const std::string& path, const ShaderProgram::Location& location)
		{
			hasGlobalMat = false;
			loadFromFile(path, location);
		}

		// can set material if the file doesn't have it
		Model(const std::string& path, const ShaderProgram::Location& location, const Material& material)
		{
			this->material = material;
			hasGlobalMat = true;
			loadFromFile(path, location);
		}

		void draw(const ShaderProgram::Location& location) const;

	private:
		void loadFromFile(const std::string& path, const ShaderProgram::Location& location);
		void processNode(const aiNode* node, const aiScene* scene, const ShaderProgram::Location& location);
		Mesh processMesh(const aiMesh* mesh, const aiScene* scene, const ShaderProgram::Location& location);

		std::vector<Mesh> meshes;
		std::vector<Texture> textures;
		Material material;
		bool hasGlobalMat = false;
	};
}