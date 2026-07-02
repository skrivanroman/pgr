#pragma once

#include <glad/glad.h>
#include <stb_image.h>

#include <iostream>
#include <string>

inline GLuint loadSRGBTexture(const std::string& path)
{
  int width, height, channels;

  stbi_set_flip_vertically_on_load(true);

  unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
  if (!data)
  {
    std::cerr << "Failed to load image: " << path << '\n';
    return 0;
  }

  GLenum format;
  GLenum internalFormat;

  if (channels == 3)
  {
    format = GL_RGB;
    internalFormat = GL_SRGB;
  }
  else if (channels == 4)
  {
    format = GL_RGBA;
    internalFormat = GL_SRGB_ALPHA;
  }
  else
  {
    std::cerr << "Unsupported image format!\n";
    stbi_image_free(data);
    return 0;
  }

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);

  glGenerateMipmap(GL_TEXTURE_2D);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glBindTexture(GL_TEXTURE_2D, 0);

  stbi_image_free(data);

  return texture;
}
#ifdef PLATFORM_WINDOWS
#include <Windows.h>

#include <filesystem>

namespace skrivrom
{
// utils for getting correct path (windows only)
std::filesystem::path getProjectRoot()
{
  char buffer[MAX_PATH];
  DWORD length = GetModuleFileNameA(nullptr, buffer, MAX_PATH);
  auto path = std::filesystem::path(std::string_view(buffer, length));
  path = path.parent_path().parent_path();
  return path.string();
}

std::string getAssetDir()
{
  auto root = getProjectRoot();
  root.append("assets");
  return root.string() + '\\';
}

std::string getShaderDir()
{
  auto root = getProjectRoot();
  root.append("src");
  root.append("shaders");
  return root.string() + '\\';
}
}  // namespace skrivrom
#elifdef PLATFORM_LINUX

#include <string>

namespace skrivrom
{

inline std::string getAsset(const std::string& itemPath)
{
  std::string path(ASSET_PATH);
  return path + '/' + itemPath;
}

inline std::string getShader(const std::string& itemPath)
{
  std::string path(SHADER_PATH);
  return path + '/' + itemPath;
}

}  // namespace skrivrom
#endif
