#pragma once

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

std::string getAsset(const std::string& itemPath)
{
  std::string path(ASSET_PATH);
  return path + itemPath;
}

std::string getShader(const std::string& itemPath)
{
  std::string path(SHADER_PATH);
  return path + itemPath;
}

}  // namespace skrivrom
#endif
