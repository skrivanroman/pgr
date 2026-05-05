#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
	glm::vec3 postion, front, up, worldUp, right;
	float fov;
	float speed, moveSpeed;
	uint32_t winWidth, winHeight;

	Camera(const glm::vec3& position, const glm::vec3& front, float speed, float fov, float winWidth, float winHeight)
	{
		this->postion = position;
		this->front = front;
		this->worldUp = { 0.0f, 1.0f, 0.0f };
		this->speed = speed;
		this->moveSpeed = speed * 10.0f;
		perspective = glm::perspective(glm::radians(fov), winWidth / winHeight, 0.01f, 1000.0f);
		this->winWidth = winWidth;
		this->winHeight = winHeight;
		yaw = -90.0f;
		pitch = 0.0f;
		right = glm::normalize(glm::cross(front, worldUp));
		up = glm::normalize(glm::cross(right, front));
		ignoreWarp = false;
	}

	void processKeyboard(unsigned char key, int x, int y) {
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
		case 'q':
		case 27:
			exit(0);
			break;
		}

		glutPostRedisplay(); 
	}

	void processMouse(int x, int y) {
		if (ignoreWarp) 
		{
			ignoreWarp = false;
			return;
		}

		float centerX = winWidth / 2.0f;
		float centerY = winHeight / 2.0f;

		float xoffset = x - centerX;
		float yoffset = centerY - y;

		float sensitivity = 0.1f;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		yaw += xoffset;
		pitch += yoffset;

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

		ignoreWarp = true;
		glutWarpPointer(winWidth / 2, winHeight / 2);

		glutPostRedisplay(); 
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
	bool ignoreWarp;
};