#pragma once

#include "pgr.h"

class ShaderProgram
{
public:
	struct DirLightLoc
	{
		GLint direction;
		GLint ambient;
		GLint diffuse;
		GLint specular;
	};

	struct PointLightLoc
	{
		GLint position;
		GLint ambient;
		GLint diffuse;
		GLint specular;
		GLint constant;
		GLint linear;
	};

	struct SpotLightLoc
	{
		GLint position;
		GLint direction;
		GLint ambient;
		GLint diffuse;
		GLint specular;
		GLint cutOff;
		GLint outerCutOff;
		GLint constant;
		GLint linear;
	};

	struct Location 
	{
		GLint postion;
		GLint normal;
		GLint texCoords;
		GLint PV;
		GLint model;
		GLint ambient;
		GLint diffuse;
		GLint specular;
		GLint shininess;
		GLint sampler;
		GLint hasTexture;
		GLint viewPos;
		DirLightLoc dirLight;
		SpotLightLoc spotLight;
		PointLightLoc pointLight;
	};

	Location location;

	ShaderProgram() {}

	ShaderProgram(const std::string& vertexPath, const std::string& fragmentPath)
	{
		std::vector<GLuint> shaders;
		shaders.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, vertexPath));
		shaders.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, fragmentPath));

		program = pgr::createProgram(shaders);

		location.postion = glGetAttribLocation(program, "position");
		location.normal = glGetAttribLocation(program, "normal");
		location.texCoords = glGetAttribLocation(program, "texCoords");
		location.PV = glGetUniformLocation(program, "PV");
		location.model = glGetUniformLocation(program, "model"); 
		location.ambient = glGetUniformLocation(program, "material.ambient");
		location.diffuse = glGetUniformLocation(program, "material.diffuse");
		location.specular = glGetUniformLocation(program, "material.specular");
		location.shininess = glGetUniformLocation(program, "material.shininess");
		location.hasTexture = glGetUniformLocation(program, "material.hasTexture");
		location.sampler = glGetUniformLocation(program, "sampler");
		location.viewPos = glGetUniformLocation(program, "viewPos");
		location.dirLight.direction = glGetUniformLocation(program, "dirLight.direction");
		location.dirLight.ambient = glGetUniformLocation(program, "dirLight.ambient");
		location.dirLight.diffuse = glGetUniformLocation(program, "dirLight.diffuse");
		location.dirLight.specular = glGetUniformLocation(program, "dirLight.specular");
		location.pointLight.position = glGetUniformLocation(program, "pointLight.position");
		location.pointLight.ambient = glGetUniformLocation(program, "pointLight.ambient");
		location.pointLight.diffuse = glGetUniformLocation(program, "pointLight.diffuse");
		location.pointLight.specular = glGetUniformLocation(program, "pointLight.specular");
		location.pointLight.constant = glGetUniformLocation(program, "pointLight.constant");
		location.pointLight.linear = glGetUniformLocation(program, "pointLight.linear");
		location.spotLight.position = glGetUniformLocation(program, "spotLight.position");
		location.spotLight.direction = glGetUniformLocation(program, "spotLight.direction");
		location.spotLight.ambient = glGetUniformLocation(program, "spotLight.ambient");
		location.spotLight.diffuse = glGetUniformLocation(program, "spotLight.diffuse");
		location.spotLight.specular = glGetUniformLocation(program, "spotLight.specular");
		location.spotLight.cutOff = glGetUniformLocation(program, "spotLight.cutOff");
		location.spotLight.outerCutOff = glGetUniformLocation(program, "spotLight.outerCutOff");
		location.spotLight.constant = glGetUniformLocation(program, "spotLight.constant");
		location.spotLight.linear = glGetUniformLocation(program, "spotLight.linear");
	}

	void use() const
	{
		glUseProgram(program);
	}

private:
	GLuint program;
};