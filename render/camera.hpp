#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "transform.hpp"

struct camera_t
{
	transform_t transform;
	float fov;
	float aspect;
	float near;
	float far;
	glm::mat4 proj;
	glm::mat4 proj_inv;

	camera_t(transform_t& transform, float fov, float aspect, float near, float far)
	{
		this->transform = transform;
		this->fov = fov;
		this->aspect = aspect;
		this->near = near;
		this->far = far;
		this->proj = glm::perspective(fov, aspect, near, far);
		this->proj_inv = glm::inverse(proj);
	}

	glm::mat4 make_view()
	{
		glm::vec3 forward = this->transform.orientation * glm::vec3(0,0,1);
		glm::vec3 center = this->transform.position + forward;
		glm::vec3 up = this->transform.orientation * glm::vec3(0,1,0);
		return glm::lookAt(this->transform.position, center, up);
	}
};

