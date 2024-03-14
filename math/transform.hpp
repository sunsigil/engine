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

	transform_t()
	{
		this->scale = glm::vec3(1);
		this->orientation = glm::quat(glm::vec3(0));
		this->position = glm::vec3(0);
		this->velocity = glm::vec3(0);
		this->acceleration = glm::vec3(0);
	}

	transform_t(glm::vec3 position) : transform_t()
	{
		this->position = position;
	}

	transform_t(glm::vec3 scale, glm::quat orientation, glm::vec3 position) : transform_t()
	{
		this->scale = scale;
		this->orientation = orientation;
		this->position = position;
	}

	glm::mat4 make_model()
	{
		glm::mat4 S = glm::scale(this->scale);
		glm::mat4 R = glm::toMat4(this->orientation);
		glm::mat4 T = glm::translate(this->position);
		return T*R*S;
	}
	glm::vec3 make_forward()
	{ return this->orientation * glm::vec3(0,0,1); }
	glm::vec3 make_up()
	{ return this->orientation * glm::vec3(0,1,0); }
	glm::vec3 make_right()
	{ return this->orientation * glm::vec3(1,0,0); }
};

void euler_integrate(transform_t& transform, float dt)
{
	transform.velocity += transform.acceleration * dt;
	transform.position += transform.velocity * dt;
}


