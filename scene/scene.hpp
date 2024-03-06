#pragma once

#include <iostream>
#include <string>

#include "transform.hpp"
#include "renderer.hpp"

#define SCENE_MAX_CAPACITY 10485076

struct scene_t
{
	int capacity;
	int size;

	std::string* names;
	transform_t* transforms;
	renderer_t* renderers;
};

void scene_init(scene_t& scene, int capacity)
{
	if(scene.capacity > SCENE_MAX_CAPACITY)
	{
		std::cerr << "[scene_init] error: desired capacity " << capacity << " exceeds maximum capacity " << SCENE_MAX_CAPACITY << std::endl;
		capacity = SCENE_MAX_CAPACITY;
	}

	std::string* names = new std::string[capacity];
	transform_t* transforms = new transform_t[capacity];
	renderer_t* renderers = new renderer_t[capacity];

	scene.capacity = capacity;
	scene.size = 0;
	scene.names = names;
	scene.transforms = transforms;
	scene.renderers = renderers;
}

void scene_add(scene_t& scene, std::string name, transform_t transform, renderer_t renderer)
{
	if(scene.size == scene.capacity)
	{
		std::cerr << "[scene_add] error: scene is at max capacity" << std::endl;
		return;
	}
	
	scene.names[scene.size] = name;
	scene.transforms[scene.size] = transform;
	scene.renderers[scene.size] = renderer;
	scene.size += 1;
}

void scene_dispose(scene_t& scene)
{
	delete[] scene.names;
	delete[] scene.transforms;
	delete[] scene.renderers;
}
