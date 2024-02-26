#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

struct transform_t
{
	glm::vec3 scale;
	glm::quat orientation;
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;
};

void transform_init(transform_t& transform)
{
	transform.scale = glm::vec3(1);
	transform.orientation = glm::quat(glm::vec3(0));
	transform.position = glm::vec3(0);
	transform.velocity = glm::vec3(0);
	transform.acceleration = glm::vec3(0);
}

void transform_init(transform_t& transform, glm::vec3 scale, glm::quat orientation, glm::vec3 position)
{
	transform_init(transform);
	transform.scale = scale;
	transform.orientation = orientation;
	transform.position = position;
}

glm::mat4 make_model(transform_t& transform)
{
	glm::mat4 S = glm::scale(transform.scale);
	glm::mat4 R = glm::toMat4(transform.orientation);
	glm::mat4 T = glm::translate(transform.position);
	return T*R*S;
}

glm::vec3 make_forward(transform_t& transform)
{ return transform.orientation * glm::vec3(0,0,1); }
glm::vec3 make_up(transform_t& transform)
{ return transform.orientation * glm::vec3(0,1,0); }
glm::vec3 make_right(transform_t& transform)
{ return transform.orientation * glm::vec3(1,0,0); }

void euler_integrate(transform_t& transform, float dt)
{
	transform.velocity += transform.acceleration * dt;
	transform.position += transform.velocity * dt;
}

