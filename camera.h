#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const float PITCH = 0.0f;
const float YAW = -90.0f;

const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;


enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

class Camera
{
	public:

		// Fields
		float pitch;
		float yaw;

		float speed;
		float sensitivity;
		float zoom;

		glm::vec3 cameraPos;
		glm::vec3 cameraFront;
		glm::vec3 cameraUp;


		// Initializer
		Camera(
			glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f),
			glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f),
			float pitch = PITCH,
			float yaw = YAW,
			float speed = SPEED,
			float sensitivity = SENSITIVITY,
			float zoom = ZOOM
		);

		//
		glm::mat4 GetViewMatrix();
		void ProcessKeyboard(Camera_Movement direction, float deltaTime);
		void ProcessMouseMovement(float xoffset, float yoffset);
		void ProccessMouseScroll(float yoffset);

};

