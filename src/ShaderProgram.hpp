#pragma once

#include <glad/glad.h>

#include <concepts>
#include <fstream>
#include <vector>

namespace skrivrom
{
// contains opengl shader program and it's locations

struct ILocation
{
  virtual ~ILocation() = default;
  virtual void getLocations(GLuint program) = 0;
};

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

struct Location : public ILocation
{
  GLint position;
  GLint normal;
  GLint texCoords;
  GLint PV;
  GLint model;
  GLint ambient;
  GLint diffuse;
  GLint specular;
  GLint shininess;
  GLint alpha;
  GLint sampler;
  GLint sampler2;
  GLint hasTwoTextures;
  GLint fogOn;
  GLint flashlightOn;
  GLint time;
  GLint hasTexture;
  GLint fogTexture;
  GLint resolution;
  GLint viewPos;
  DirLightLoc dirLight;
  SpotLightLoc spotLight;
  PointLightLoc pointLight;

  void getLocations(GLuint program) override
  {
    position = glGetAttribLocation(program, "position");
    normal = glGetAttribLocation(program, "normal");
    texCoords = glGetAttribLocation(program, "texCoords");
    PV = glGetUniformLocation(program, "PV");
    model = glGetUniformLocation(program, "model");
    ambient = glGetUniformLocation(program, "material.ambient");
    diffuse = glGetUniformLocation(program, "material.diffuse");
    specular = glGetUniformLocation(program, "material.specular");
    shininess = glGetUniformLocation(program, "material.shininess");
    hasTexture = glGetUniformLocation(program, "material.hasTexture");
    alpha = glGetUniformLocation(program, "material.alpha");
    sampler = glGetUniformLocation(program, "sampler");
    sampler2 = glGetUniformLocation(program, "sampler2");
    hasTwoTextures = glGetUniformLocation(program, "hasTwoTextures");
    fogTexture = glGetUniformLocation(program, "fogTexture");
    resolution = glGetUniformLocation(program, "resolution");
    viewPos = glGetUniformLocation(program, "viewPos");
    fogOn = glGetUniformLocation(program, "fogOn");
    flashlightOn = glGetUniformLocation(program, "flashlightOn");
    time = glGetUniformLocation(program, "time");
    dirLight.direction = glGetUniformLocation(program, "dirLight.direction");
    dirLight.ambient = glGetUniformLocation(program, "dirLight.ambient");
    dirLight.diffuse = glGetUniformLocation(program, "dirLight.diffuse");
    dirLight.specular = glGetUniformLocation(program, "dirLight.specular");
    pointLight.position = glGetUniformLocation(program, "pointLight.position");
    pointLight.ambient = glGetUniformLocation(program, "pointLight.ambient");
    pointLight.diffuse = glGetUniformLocation(program, "pointLight.diffuse");
    pointLight.specular = glGetUniformLocation(program, "pointLight.specular");
    pointLight.constant = glGetUniformLocation(program, "pointLight.constant");
    pointLight.linear = glGetUniformLocation(program, "pointLight.linear");
    spotLight.position = glGetUniformLocation(program, "spotLight.position");
    spotLight.direction = glGetUniformLocation(program, "spotLight.direction");
    spotLight.ambient = glGetUniformLocation(program, "spotLight.ambient");
    spotLight.diffuse = glGetUniformLocation(program, "spotLight.diffuse");
    spotLight.specular = glGetUniformLocation(program, "spotLight.specular");
    spotLight.cutOff = glGetUniformLocation(program, "spotLight.cutOff");
    spotLight.outerCutOff = glGetUniformLocation(program, "spotLight.outerCutOff");
    spotLight.constant = glGetUniformLocation(program, "spotLight.constant");
    spotLight.linear = glGetUniformLocation(program, "spotLight.linear");
  }
};

struct BarLocation : public ILocation
{
  GLint position;
  GLint texCoords;
  GLint time;

  void getLocations(GLuint program) override
  {
    position = glGetAttribLocation(program, "position");
    texCoords = glGetAttribLocation(program, "texCoords");
    time = glGetUniformLocation(program, "time");
  }
};

struct WaterLocation : public ILocation
{
  GLint position;
  GLint texCoords;
  GLint PV;
  GLint model;
  GLint texTransform;
  GLint sampler;

  void getLocations(GLuint program) override
  {
    position = glGetAttribLocation(program, "position");
    texCoords = glGetAttribLocation(program, "texCoords");
    sampler = glGetUniformLocation(program, "sampler");
    PV = glGetUniformLocation(program, "PV");
    model = glGetUniformLocation(program, "model");
    texTransform = glGetUniformLocation(program, "texTransform");
  }
};

struct SkyboxLocation : public ILocation
{
  GLint position;
  GLint PV;
  GLint sampler;

  void getLocations(GLuint program) override
  {
    position = glGetAttribLocation(program, "position");
    sampler = glGetUniformLocation(program, "sampler");
    PV = glGetUniformLocation(program, "PV");
  }
};

struct SpriteLocation : public ILocation
{
  GLint PV;
  GLint model;
  GLint spriteTexture;
  GLint uvScale;
  GLint uvOffset;

  void getLocations(GLuint program) override
  {
    spriteTexture = glGetUniformLocation(program, "spriteTexture");
    PV = glGetUniformLocation(program, "PV");
    model = glGetUniformLocation(program, "model");
    uvScale = glGetUniformLocation(program, "uvScale");
    uvOffset = glGetUniformLocation(program, "uvOffset");
  }
};

template <std::derived_from<ILocation> L>
class ShaderProgram
{
 public:
  L location;

  ShaderProgram() : program(0) {};

  ShaderProgram(const ShaderProgram&) = default;
  ShaderProgram& operator=(const ShaderProgram&) = default;
  ShaderProgram(ShaderProgram&&) = default;
  ShaderProgram& operator=(ShaderProgram&&) = default;

  ShaderProgram(const std::string& vertexPath, const std::string& fragmentPath)
  {
    std::string vertexCode = loadShaderCode(vertexPath);
    const char* vertexCodeChar = vertexCode.c_str();
    GLuint vertexHandle = glCreateShaderProgramv(GL_VERTEX_SHADER, 1, &vertexCodeChar);

    std::string fragCode = loadShaderCode(vertexPath);
    const char* fragCodeChar = fragCode.c_str();
    GLuint fragHandle = glCreateShaderProgramv(GL_FRAGMENT_SHADER, 1, &fragCodeChar);

    program = glCreateProgram();
    glAttachShader(program, vertexHandle);
    glAttachShader(program, fragHandle);

    location.getLocations(program);
    glUseProgram(0);
  }

  ~ShaderProgram()
  {
    glDeleteProgram(program);
  }

  void use() const
  {
    glUseProgram(program);
  }

 private:
  std::string loadShaderCode(const std::string& path)
  {
    std::ifstream shaderFile(path, std::ios::ate | std::ios::binary);
    size_t size = static_cast<size_t>(shaderFile.tellg());
    std::vector<char> data(size);

    shaderFile.seekg(0);
    shaderFile.read(data.data(), size);

    std::string str(data.data());
    return str;
  }

  GLuint program;
};
}  // namespace skrivrom
