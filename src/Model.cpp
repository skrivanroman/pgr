#include "Model.hpp"
#include <iostream>

void Model::loadFromFile(const std::string& path, const ShaderProgram::Location& location)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
        aiProcess_FlipUVs |
        aiProcess_CalcTangentSpace
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
    {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << '\n';
        return;
    }

    std::cerr << "Meshes found: " << scene->mNumMeshes << ".\n";

    const aiMesh* mesh = scene->mMeshes[0];

    if (!mesh->HasNormals())
    {
        std::cerr << "Mesh has no normals\n";
        return;
    }

    directory = path.substr(0, path.find_last_of('\\'));

    processNode(scene->mRootNode, scene, location);
}

void Model::processNode(const aiNode* node, const aiScene* scene, const ShaderProgram::Location& location) 
{

    for (uint32_t i = 0; i < node->mNumMeshes; ++i) 
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene, location));
    }

    for (uint32_t i = 0; i < node->mNumChildren; ++i) 
    {
        processNode(node->mChildren[i], scene, location);
    }
}

Mesh Model::processMesh(const aiMesh* mesh, const aiScene* scene, const ShaderProgram::Location& location)
{
    std::vector<Vertex> vertices(mesh->mNumVertices);

    for (uint32_t i = 0; i < mesh->mNumVertices; ++i) 
    {
        vertices[i].position.x = mesh->mVertices[i].x;
        vertices[i].position.y = mesh->mVertices[i].y;
        vertices[i].position.z = mesh->mVertices[i].z;

		vertices[i].normal.x = mesh->mNormals[i].x;
		vertices[i].normal.y = mesh->mNormals[i].y;
		vertices[i].normal.z = mesh->mNormals[i].z;

        if (mesh->mTextureCoords[0]) 
        {
            vertices[i].texCoords.x = mesh->mTextureCoords[0][i].x;
            vertices[i].texCoords.y = mesh->mTextureCoords[0][i].y;
        }
        else 
        {
            vertices[i].texCoords = glm::vec2(0.0f, 0.0f);
        }

    }

    std::vector<uint32_t> indices;

    for (uint32_t i = 0; i < mesh->mNumFaces; ++i) 
    {
        aiFace face = mesh->mFaces[i];
        for (uint32_t j = 0; j < face.mNumIndices; ++j) 
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    aiMaterial* assimpMaterial = scene->mMaterials[mesh->mMaterialIndex];
    aiColor3D color;
    Material material;

    if (assimpMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) 
    {
        material.diffuse = glm::vec3(color.r, color.g, color.b);
    }

    if (assimpMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS) 
    {
        material.ambient = glm::vec3(color.r, color.g, color.b);
    }

    if (assimpMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS) 
    {
        material.specular = glm::vec3(color.r, color.g, color.b);
    }

    float shininess;
    if (assimpMaterial->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS) 
    {
        material.shininess = shininess;
    }

    if (assimpMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) 
    {
        aiString name;

        aiReturn texFound = assimpMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &name);
        std::string texturePath = directory + '\\' + name.data;

        bool alreadyLoaded = false;
        for (const auto& tex : textures)
        {
            if (tex.path == texturePath)
            {
                alreadyLoaded = true;
                material.texture = tex.id;
                material.hasTexture = true;
                break;
            }
        }

        if (!alreadyLoaded)
        {
            std::cout << "Loading texture file: " << name.data << std::endl;
            material.texture = pgr::createTexture(texturePath);
            material.hasTexture = true;

            textures.push_back(Texture{material.texture, texturePath});
        }
    }
    else
    {
        material.hasTexture = false;
        std::cout << "No texture found\n";
    }

    return Mesh(vertices, indices, material, location);
}

void Model::draw(const ShaderProgram::Location& location) const
{ 
	glm::mat4 model = glm::mat4(1.0f);

	model = glm::translate(model, transform.position);

	model = glm::rotate(model, glm::radians(transform.rotation.x), glm::vec3(1, 0, 0));
	model = glm::rotate(model, glm::radians(transform.rotation.y), glm::vec3(0, 1, 0));
	model = glm::rotate(model, glm::radians(transform.rotation.z), glm::vec3(0, 0, 1));

	model = glm::scale(model, transform.scale);

    glUniformMatrix4fv(location.model, 1, GL_FALSE, glm::value_ptr(model));

    for (const Mesh& mesh : meshes)
    {
        mesh.draw(location);
    }
}
