#include "Camera.h"
#include <SDL3/SDL.h>
#include <iostream>

const float YAW = -90.f;
const float PITCH = 0.f;
const float SPEED = 25.0f;
const float SENSITIVITY = 0.15f;

void Camera::ProcessMouseMovements()
{

	SDL_PumpEvents();
	float x = 0.f, y = 0.f;
	const SDL_MouseButtonFlags& Button = SDL_GetMouseState(&x, &y);

	static bool first = true;
	static float lastX, lastY;

	if (first)
	{
		lastX = x; lastY = y;
		first = false;
	}

	float xOffset = 0, yOffset = 0;
	if (Button == SDL_BUTTON_RMASK)
	{
		xOffset = x - lastX;
		yOffset = y - lastY;
	}

	lastX = x; lastY = y;

	Yaw += (xOffset * SENSITIVITY);
	Pitch += (yOffset * SENSITIVITY);

	if (Pitch > 89.f) Pitch = 89.f;
	if (Pitch < -89.f) Pitch = -89.f;

	UpdateVectors();

}

void Camera::UpdateVectors()
{
	glm::vec3 front;
	front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	front.y = sin(glm::radians(Pitch));
	front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	Front = glm::normalize(front);

	Right = glm::normalize(glm::cross(Front, WorldUp));
	Up = glm::normalize(glm::cross(Right, Front));

}

void Camera::ProcessKeyboardMovements(float deltaTime)
{
	
	const bool* Key = SDL_GetKeyboardState(nullptr);
	
	float velocity = SPEED * deltaTime;

	if (Key[SDL_SCANCODE_W])
	{
		Position += Front * velocity;
	}
	else if (Key[SDL_SCANCODE_S])
	{
		Position -= Front * velocity;
	}

	if (Key[SDL_SCANCODE_D])
	{
		Position += Right * velocity;
	}
	else if (Key[SDL_SCANCODE_A])
	{
		Position -= Right * velocity;
	}
}

Camera::Camera()
{
	projection = glm::perspective(glm::radians(45.0f), ViewSize.x / ViewSize.y, 0.1f, 200.0f);
	view = glm::mat4(1.f);

	Yaw = YAW; Pitch = PITCH;
	UpdateVectors();
}

Camera::~Camera()
{
}

glm::mat4 Camera::getView(float deltaTime)
{
	
	ProcessMouseMovements();
	ProcessKeyboardMovements(deltaTime);

	static glm::vec2 lastSize = glm::vec2(0, 0);
	if (lastSize != ViewSize)
	{
		projection = glm::perspective(glm::radians(45.0f), ViewSize.x / ViewSize.y, 0.1f, 200.0f);
		lastSize = ViewSize;
	}

	glm::mat4 v = glm::lookAt(Position, Position + Front, Up);
	CamPosition = Position;

	return v;

}
