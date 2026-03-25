#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

inline glm::vec3 CamPosition{ 0, 0, 0 };
inline glm::vec2 ViewSize = glm::vec2(1080, 620);

class Camera
{

private:

	glm::vec3 Position{ 0, 10, 4 };
	glm::vec3 Front{ 0.f, 0.f, -1.f };
	glm::vec3 Right{ 0.f, 0.f, 0.f };
	glm::vec3 Up{ 0.f, 1.f, 0.f };
	glm::vec3 WorldUp{ 0.f, 1.f, 0.f };

	float Pitch = 0.f;
	float Yaw = -90.f;

	glm::mat4 projection;
	glm::mat4 view;

	void ProcessMouseMovements();
	void UpdateVectors();

	void ProcessKeyboardMovements(float deltaTime);

public:
	Camera();
	~Camera();

	inline glm::vec3 GetViewPos() { return Position; }
	glm::mat4 getView(float deltaTime);
	glm::mat4 getProj() { return projection; }

};


#endif