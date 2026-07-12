#include "camera.h"
#include <glad/glad.h>
#include <glfw3.h> 


Camera::Camera(
    glm::vec3 cameraPos,
    glm::vec3 cameraFront,
    glm::vec3 cameraUp,
    float pitch,
    float yaw,
    float speed,
    float sensitivity,
    float zoom
)
	: pitch(PITCH),
	yaw(YAW),
	speed(speed),
	sensitivity(sensitivity),
	zoom(ZOOM),
	cameraPos(cameraPos),
	cameraFront(cameraFront),
	cameraUp(cameraUp)
{}


glm::mat4 Camera::GetViewMatrix() {
	return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}


void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime) {
	if (direction == FORWARD) {
		cameraPos += cameraFront * speed * deltaTime;
	}
	if (direction == BACKWARD) {
		cameraPos -= cameraFront * speed * deltaTime;
	}
	if (direction == RIGHT) {
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * speed * deltaTime;
	}
	if (direction == LEFT) {
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * speed * deltaTime;
	}
	return;
}


void Camera::ProcessMouseMovement(float xoffset, float yoffset) {
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f) {
		pitch = 89.0f;
	}
	if (pitch < -89.0f) {
		pitch = -89.0f;
	}

	glm::vec3 direction;

	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	cameraFront = glm::normalize(direction);
}


void Camera::ProccessMouseScroll(float yoffset) {
	zoom -= yoffset;

	if (zoom > 45.0f) {
		zoom = 45.0f;
	}
	if (zoom < 1.0f) {
		zoom = 1.0f;
	}
}

//void updateCameraVectors();
