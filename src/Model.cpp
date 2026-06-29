#include "Model.hpp"

#include <iostream>

namespace skrivrom
{
void Model::loadFromFile(const std::string& path, const Location& location)
{
  Assimp::Importer importer;
  const aiScene* scene =
      importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                                  aiProcess_JoinIdenticalVertices | aiProcess_PreTransformVertices
                        // aiProcess_FlipUVs |
                        // aiProcess_CalcTangentSpace
      );

  directory = path.substr(0, path.find_last_of('\\'));

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
  {
    std::cerr << "Assimp error when loading " << directory << ' ' << importer.GetErrorString()
              << '\n';
    return;
  }

  std::cout << directory << " meshes found: " << scene->mNumMeshes << ".\n";

  std::string extension = path.substr(path.find_last_of("."));
  if (extension == ".obj")
  {
    transform.importCorr = glm::quat(1, 0, 0, 0);
  }
  else if (extension == ".gltf")
  {
    transform.importCorr = glm::quat(1, 0, 0, 0);
    // transform.importCorr = glm::angleAxis(glm::radians(-90.0f), glm::vec3(1, 0, 0));
  }
  else
  {
    std::cerr << "Unsupported model extension: " << extension << '\n';
    return;
  }

  const aiMesh* mesh = scene->mMeshes[0];
  processNode(scene->mRootNode, scene, location);

  modelBounds.width = modelBounds.maxPoint.x - modelBounds.minPoint.x;
  modelBounds.height = modelBounds.maxPoint.y - modelBounds.minPoint.y;
  modelBounds.depth = modelBounds.maxPoint.z - modelBounds.minPoint.z;

  std::cout << "Default dimensions are: ( " << modelBounds.width << ", " << modelBounds.height
            << ", " << modelBounds.depth << " )\n";

  modelBounds.center = (modelBounds.minPoint + modelBounds.maxPoint) * 0.5f;

  std::cout << "center: ( " << modelBounds.center.x << ", " << modelBounds.center.y << ", "
            << modelBounds.center.z << " ) \n\n";
}

void Model::processNode(const aiNode* node, const aiScene* scene, const Location& location)
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

Mesh Model::processMesh(const aiMesh* mesh, const aiScene* scene, const Location& location)
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

    modelBounds.minPoint.x = std::min(modelBounds.minPoint.x, vertices[i].position.x);
    modelBounds.minPoint.y = std::min(modelBounds.minPoint.y, vertices[i].position.y);
    modelBounds.minPoint.z = std::min(modelBounds.minPoint.z, vertices[i].position.z);

    modelBounds.maxPoint.x = std::max(modelBounds.maxPoint.x, vertices[i].position.x);
    modelBounds.maxPoint.y = std::max(modelBounds.maxPoint.y, vertices[i].position.y);
    modelBounds.maxPoint.z = std::max(modelBounds.maxPoint.z, vertices[i].position.z);
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
      std::cout << "Loading texture file: " << name.data << '\n';
      material.texture = pgr::createTexture(texturePath);
      material.hasTexture = true;

      textures.push_back(Texture{.id = material.texture, .path = texturePath});
    }
  }
  else
  {
    material.hasTexture = false;
    std::cout << "No texture found\n";
  }

  if (!hasGlobalMat)
  {
    return Mesh(vertices, indices, material, location);
  }

  this->material.hasTexture = material.hasTexture;
  this->material.texture = material.texture;
  return Mesh(vertices, indices, this->material, location);
}

void Model::draw(const Location& location) const
{
  if (!visible)
  {
    return;
  }

  glm::mat4 model = transform.calcModel();
  glUniformMatrix4fv(location.model, 1, GL_FALSE, glm::value_ptr(model));

  for (const Mesh& mesh : meshes)
  {
    mesh.draw(location);
  }
}
}  // namespace skrivrom
