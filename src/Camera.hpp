#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "GLFW/glfw3.h"

namespace skrivrom
{
// camera class for static, free and dynamic cameras
class Camera
{
 public:
  glm::vec3 postion, front, up, worldUp, right;
  float fov;
  float speed, moveSpeed;
  uint32_t winWidth, winHeight;

  Camera()
  {
  }

  Camera(const glm::vec3& position, const glm::vec3& front, float speed, float fov, float winWidth,
         float winHeight)
  {
    this->postion = position;
    this->front = front;
    this->worldUp = {0.0f, 1.0f, 0.0f};
    this->speed = speed;
    this->moveSpeed = speed * 5.0f;
    this->fov = fov;
    perspective = glm::perspective(glm::radians(fov), winWidth / winHeight, 0.1f, 3000.0f);
    this->winWidth = winWidth;
    this->winHeight = winHeight;
    yaw = -90.0f;
    pitch = 0.0f;
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
    firstMove = false;
  }

  void processKeyboard(unsigned char key)
  {
    switch (key)
    {
      case 'w':
        postion += moveSpeed * front;
        break;
      case 's':
        postion -= moveSpeed * front;
        break;
      case 'a':
        postion -= right * moveSpeed;
        break;
      case 'd':
        postion += right * moveSpeed;
        break;
      case 'x':
        postion += up * moveSpeed;
        break;
      case 'z':
        postion -= up * moveSpeed;
        break;
    }
  }

  void processMouse(GLFWwindow* window, double x, double y, float deltaTime)
  {
    if (firstMove)
    {
      lastX = x;
      lastY = y;
      firstMove = false;
    }

    float xoffset = x - lastX;
    float yoffset = lastY - y;
    lastX = x;
    lastY = y;

    float sensitivity = 5.0f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset * deltaTime;
    pitch += yoffset * deltaTime;

    if (pitch > 89.0f)
      pitch = 89.0f;
    if (pitch < -89.0f)
      pitch = -89.0f;

    glm::vec3 dir;
    dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    dir.y = sin(glm::radians(pitch));
    dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(dir);

    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
  }

  glm::mat4 calculatePV() const
  {
    return perspective * glm::lookAt(postion, postion + front, up);
  }

  glm::mat4 getSkyboxPV() const
  {
    glm::mat4 view = glm::lookAt(postion, postion + front, up);
    return perspective * glm::mat4(glm::mat3(view));
  }

  const glm::mat4& getPerspective() const
  {
    return perspective;
  }

  // new window dimensions
  void handleResize(uint32_t width, uint32_t height)
  {
    float aspect = (float)width / (float)height;

    perspective = glm::perspective(glm::radians(fov), aspect, 0.01f, 1000.0f);
    winWidth = width;
    winHeight = height;
  }

 private:
  glm::mat4 perspective;
  float yaw;
  float pitch;
  bool firstMove;
  double lastX, lastY;
};
}  // namespace skrivrom
