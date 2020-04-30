#pragma once

#include <glad/include/glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

//Enum for different camera movements
enum Camera_Actions
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

//Inital camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 5000.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;
const float CAMERADISTANCE = 50.0f;

class Camera
{
public:
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;

	//Euler
	float Yaw;
	float Pitch;

	//Camera variables
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(Position, Position + Front, Up);
	}

	glm::mat4 GetViewModel(glm::mat4 mat)
	{
		glm::vec3 pos = glm::vec3(mat[0][3], mat[1][3], mat[2][3]);
		glm::vec3 front = glm::vec3(mat[0][2], mat[1][2], mat[2][2]);
		glm::vec3 up = glm::vec3(mat[0][1], mat[1][1], mat[2][1]);
		return glm::lookAt(pos, pos + front, up);
	}

	void ProcessKeyboard(Camera_Actions action, float deltaTime)
	{
		float velocity = MovementSpeed * deltaTime;
		if (action == FORWARD)
			Position += Front * velocity;
		if (action == BACKWARD)
			Position -= Front * velocity;
		if (action == LEFT)
			Position -= Right * velocity;
		if (action == RIGHT)
			Position += Right * velocity;
	}

	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		//Stop flipping of screen by constraining the region
		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}
		updateCameraVectors();
	}

	void ProcessMouseScroll(float yoffset)
	{
		if (Zoom >= 1.0f && Zoom <= 45.0f)
			Zoom -= yoffset;
		if (Zoom <= 1.0f)
			Zoom = 1.0f;
		if (Zoom >= 45.0f)
			Zoom = 45.0f;
	}

private:
	//Updates new camera angles from inputs
	void updateCameraVectors()
	{
		//Front
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);

		//Right
		Right = glm::normalize(glm::cross(Front, WorldUp));

		//Up
		Up = glm::normalize(glm::cross(Right, Front));
	}
};