#pragma once

#include <iostream>

#include "transform.hpp"
#include "renderer.hpp"

#define SCENE_MAX_CAPACITY 10485076

struct scene_t
{
	int capacity;
	int size;

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

	transform_t* transforms = (transform_t*) malloc(sizeof(transform_t) * capacity);
	renderer_t* renderers = (renderer_t*) malloc(sizeof(renderer_t) * capacity);

	scene.capacity = capacity;
	scene.size = 0;
	scene.transforms = transforms;
	scene.renderers = renderers;
}

void scene_add(scene_t& scene, transform_t transform, renderer_t renderer)
{
	if(scene.size == scene.capacity)
	{
		std::cerr << "[scene_add] error: scene is at max capacity" << std::endl;
		return;
	}

	scene.transforms[scene.size] = transform;
	scene.renderers[scene.size] = renderer;
	scene.size += 1;
}

void scene_dispose(scene_t& scene)
{
	free(scene.transforms);
	free(scene.renderers);
}
