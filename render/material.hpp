#pragma once

#include <glm/glm.hpp>
#include <string>
#include "texture.hpp"

typedef struct material
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
} material_t;

void material_init(material_t& material)
{
	material.Ka = glm::vec3(1);
	material.Kd = glm::vec3(0);
	material.Ks = glm::vec3(1);
	material.d = 1;
	material.halo = false;
	material.Ns = 20;
	material.map_Ka = nullptr;
	material.map_Kd = nullptr;
	material.map_Ks = nullptr;
	material.map_d = nullptr;
}
