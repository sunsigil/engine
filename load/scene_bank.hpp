#pragma once

#include "formats/json.h"
#include "scene.hpp"
#include "directory.hpp"
#include "json_basics.hpp"
#include "asset_bank.hpp"

#include <string>
#include <map>
#include <vector>


struct scene_bank_t
{
	std::map<std::string, scene_t> scenes;
};

transform_t transform_JSON_init(JSON_head_t* json)
{
	if(*json != JSON_OBJECT)
	{
		std::cerr << "[transform_JSON_init] error: expected JSON_OBJECT representation" << std::endl;
		return transform_t();
	}

	glm::vec3 scale = vec_JSON_init<3>(JSON_object_get(json, "scale"));
	glm::quat orientation = glm::quat(vec_JSON_init<3>(JSON_object_get(json, "orientation")));
	glm::vec3 position = vec_JSON_init<3>(JSON_object_get(json, "position"));

	return transform_t(scale, orientation, position);
}

renderer_t renderer_JSON_init(JSON_head_t* json, asset_bank_t& assets)
{
	if(*json != JSON_OBJECT)
	{
		std::cerr << "[renderer_JSON_init] error: expected JSON_OBJECT representation" << std::endl;
		return renderer_t();
	}

	JSON_string_t* mesh_name = (JSON_string_t*) JSON_object_get(json, "mesh");
	mesh_t* mesh = assets.meshes[mesh_name->value];
	JSON_string_t* shader_name = (JSON_string_t*) JSON_object_get(json, "shader");
	shader_t* shader = assets.shaders[shader_name->value];
	JSON_string_t* material_name = (JSON_string_t*) JSON_object_get(json, "material");
	material_t* material = assets.materials[material_name->value];
	
	return renderer_t(mesh, shader, material);
}

void scene_JSON_init(scene_t& scene, JSON_head_t* json, asset_bank_t& assets)
{
	scene_init(scene, 10000);
	
	JSON_head_t* props = JSON_object_get(json, "props");
	if(props == NULL)
	{
		std::cerr << "[scene_JSON_init] error: prop array not found" << std::endl;
		return;
	}
	else if(*props != JSON_ARRAY)
	{
		std::cerr <<"[scene_JSON_init] error: expected JSON_ARRAY props representation" << std::endl;
		return;
	}

	int prop_count = ((JSON_array_t*) props)->length;
	for(int i = 0; i < prop_count; i++)
	{
		JSON_head_t* prop = JSON_array_get(props, i);

		JSON_head_t* name_json = JSON_object_get(prop, "name");
		std::string name = std::string(((JSON_string_t*) name_json)->value);

		JSON_head_t* transform_json = JSON_object_get(prop, "transform");
		transform_t transform = transform_JSON_init(transform_json);

		JSON_head_t* renderer_json = JSON_object_get(prop, "renderer");
		renderer_t renderer = renderer_JSON_init(renderer_json, assets);

		scene_add(scene, name, transform, renderer);
	}
}

void scene_bank_init(scene_bank_t& scenes, dir_node_t* dir, asset_bank_t& assets)
{
	scenes.scenes = std::map<std::string, scene_t>();

	std::vector<dir_node_t*> files = std::vector<dir_node_t*>();
	dir_pick_leaves(dir, files);

	for(int i = 0; i < files.size(); i++)
	{
		dir_node_t* file = files[i];
		if(file->extension == "scene")
		{
			std::cerr << "[scene_bank_init] loading scene: " << file->path << std::endl;
			JSON_head_t* json = JSON_read(file->path.c_str());
			scene_JSON_init(scenes.scenes[file->name], json, assets);
			JSON_dispose(json);
		}
	}
}
