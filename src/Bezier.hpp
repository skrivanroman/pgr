#pragma once

#include <glm/glm.hpp>

#include "Model.hpp"

namespace skrivrom
{

// collection of bezier curve segments
class Bezier
{
 public:
  class Segment;
  std::vector<Segment> segments;
  glm::quat lastRotation = glm::quat(1, 0, 0, 0);

  Bezier()
  {
  }

  Bezier(std::vector<Segment>&& segments)
  {
    this->segments = segments;
    for (Segment& seg : this->segments)
    {
      seg.lastRotation = &lastRotation;
    }
  }

  class Segment
  {
   public:
    glm::vec3 p0, p1, p2, p3;
    glm::quat* lastRotation = nullptr;

    glm::vec3 getPosition(float t) const
    {
      float u = 1.0f - t;

      float t2 = t * t;
      float u2 = u * u;

      float u3 = u2 * u;
      float t3 = t2 * t;

      glm::vec3 p = u3 * p0;
      p += 3 * u2 * t * p1;
      p += 3 * u * t2 * p2;
      p += t3 * p3;

      return p;
    }

    glm::vec3 getTangent(float t) const
    {
      float u = 1.0f - t;

      glm::vec3 tangent = 3.0f * u * u * (p1 - p0);
      tangent += 6.0f * u * t * (p2 - p1);
      tangent += 3.0f * t * t * (p3 - p2);

      return glm::normalize(tangent);
    }

    void moveObject(Transform& transform, float t)
    {
      transform.position = getPosition(t);

      glm::vec3 forward = getTangent(t);

      float yaw = std::atan2(forward.x, forward.z);
      glm::quat yawComponent = glm::angleAxis(yaw, glm::vec3(0.0f, 1.0f, 0.0f));

      float pitch = -std::asin(forward.y);
      glm::quat pitchComponent = glm::angleAxis(pitch, glm::vec3(1.0f, 0.0f, 0.0f));

      glm::quat alignment = glm::angleAxis(glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

      glm::quat targetRotation = glm::normalize(yawComponent * pitchComponent * alignment);

      transform.rotation = glm::slerp(*lastRotation, targetRotation, 0.2f);

      *lastRotation = transform.rotation;

      /*
      transform.position = getPosition(t);
      glm::vec3 forward = getTangent(t);

      glm::vec3 worldUp = glm::vec3(0, 1, 0);

      glm::vec3 right = glm::normalize(glm::cross(worldUp, forward));
      glm::vec3 up = glm::normalize(glm::cross(forward, right));

      glm::mat3 rotMatrix(right, up, forward);
      glm::quat targetRotation = glm::normalize(glm::quat_cast(rotMatrix));

      transform.rotation = glm::slerp(*lastRotation, targetRotation, 0.2f);

      *lastRotation = transform.rotation;
      */
    }
  };
};
}  // namespace skrivrom
