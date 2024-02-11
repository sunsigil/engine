#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

struct camera_t
{
	float fov;
	float aspect;
	float near;
	float far;
	glm::mat4 proj;
};

void camera_init(camera_t& camera, float fov, float aspect, float near, float far)
{
	camera.fov = fov;
	camera.aspect = aspect;
	camera.near = near;
	camera.far = far;
	camera.proj = glm::perspective(fov, aspect, near, far);
}

glm::mat4 make_view(camera_t& camera, glm::vec3 eye, glm::quat orientation)
{
	glm::vec3 forward = orientation * glm::vec3(0,0,1);
	glm::vec3 center = eye + forward;
	glm::vec3 up = orientation * glm::vec3(0,1,0);
	return glm::lookAt(eye, center, up);
}

