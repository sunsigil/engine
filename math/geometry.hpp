#pragma once

#include <glm/glm.hpp>

enum axis_t
{
	X,
	Y,
	Z
};

enum orient_t
{
	CW,
	CCW
};

glm::vec3 axis_normal(axis_t axis, orient_t orient)
{
	glm::vec3 normal = glm::vec3(0);
	normal[axis] = orient == CW ? 1 : -1;
	return normal;
}
