#pragma once

#include <glm/glm.hpp>
#include <string>
#include "texture.hpp"

struct material_t
{	
	glm::vec3 Ka;
	glm::vec3 Kd;
	glm::vec3 Ks;
	float d;
	bool halo;
	int Ns;
	texture_t* map_Ka;
	texture_t* map_Kd;
	texture_t* map_Ks;
	texture_t* map_d;

	material_t()
	{
		this->Ka = glm::vec3(1);
		this->Kd = glm::vec3(0);
		this->Ks = glm::vec3(1);
		this->d = 1;
		this->halo = false;
		this->Ns = 20;
		this->map_Ka = nullptr;
		this->map_Kd = nullptr;
		this->map_Ks = nullptr;
		this->map_d = nullptr;
	}
};
