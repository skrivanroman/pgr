#pragma once

#include <vector>
#include <cmath>
#include <glm/glm.hpp>

namespace skrivrom
{

    // continious Bspline curve can move camera
    class BSpline
    {
    public:
        std::vector<glm::vec3> controlPoints;

        BSpline(std::vector<glm::vec3>&& points) : controlPoints(points) {}

        glm::vec3 getPosition(size_t segmentIdx, float t) const
        {
            size_t n = controlPoints.size();
            if (n < 4) return glm::vec3(0.0f);

            glm::vec3 p0 = controlPoints[segmentIdx % n];
            glm::vec3 p1 = controlPoints[(segmentIdx + 1) % n];
            glm::vec3 p2 = controlPoints[(segmentIdx + 2) % n];
            glm::vec3 p3 = controlPoints[(segmentIdx + 3) % n];

            float t2 = t * t;
            float t3 = t2 * t;

            float f0 = (-t3 + 3.0f * t2 - 3.0f * t + 1.0f) / 6.0f;
            float f1 = (3.0f * t3 - 6.0f * t2 + 4.0f) / 6.0f;
            float f2 = (-3.0f * t3 + 3.0f * t2 + 3.0f * t + 1.0f) / 6.0f;
            float f3 = t3 / 6.0f;

            return f0 * p0 + f1 * p1 + f2 * p2 + f3 * p3;
        }

        glm::vec3 getTangent(size_t segmentIdx, float t) const
        {
            size_t n = controlPoints.size();
            if (n < 4) return glm::vec3(0.0f, 0.0f, 1.0f);

            glm::vec3 p0 = controlPoints[segmentIdx % n];
            glm::vec3 p1 = controlPoints[(segmentIdx + 1) % n];
            glm::vec3 p2 = controlPoints[(segmentIdx + 2) % n];
            glm::vec3 p3 = controlPoints[(segmentIdx + 3) % n];

            float t2 = t * t;

            float df0 = (-3.0f * t2 + 6.0f * t - 3.0f) / 6.0f;
            float df1 = (9.0f * t2 - 12.0f * t) / 6.0f;
            float df2 = (-9.0f * t2 + 6.0f * t + 3.0f) / 6.0f;
            float df3 = (3.0f * t2) / 6.0f;

            return df0 * p0 + df1 * p1 + df2 * p2 + df3 * p3;
        }

        void moveCamera(Camera& cam, float globalTime) const
        {
            size_t n = controlPoints.size();

            size_t segmentIdx = static_cast<size_t>(std::floor(globalTime)) % n;
            float t = globalTime - std::floor(globalTime);

            cam.postion = getPosition(segmentIdx, t);

            cam.front = glm::normalize(getTangent(segmentIdx, t));

            glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
            glm::vec3 right = glm::normalize(glm::cross(cam.front, worldUp));
            cam.up = glm::normalize(glm::cross(right, cam.front));
        }
    };
}