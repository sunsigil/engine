#pragma once

#include "formats/json.h"

#include "scene.hpp"
#include "transform.hpp"
#include "renderer.hpp"
#include "bank.hpp"

#include <string>
#include <map>
#include <dirent.h>
#include <sys/stat.h>

void scene_JSON_init(scene_t& scene, JSON_head_t* json, bank_t& bank)
{
	JSON_array_t* props = (JSON_array_t*) JSON_object_get(json, "props");
	if(props == NULL)
	{
		std::cerr << "[scene_JSON_init] error: prop array is null" << std::endl;
		return;
	}

	scene_init(scene, 1000);

	for(int i = 0; i < props->length; i++)
	{
		JSON_object_t* prop = (JSON_object_t*) JSON_array_get(&props->head, i);

		JSON_object_t* transform_j = (JSON_object_t*) JSON_object_get(&prop->head, "transform");
		transform_t transform;
		transform_init(transform, glm::vec3(1), glm::quat(1,0,0,0), glm::vec3(0));

		JSON_array_t* vec3_j = (JSON_array_t*) JSON_object_get(&transform_j->head, "position");
		JSON_POD_t* x = (JSON_POD_t*) JSON_array_get(&vec3_j->head, 0);
		JSON_POD_t* y = (JSON_POD_t*) JSON_array_get(&vec3_j->head, 1);
		JSON_POD_t* z = (JSON_POD_t*) JSON_array_get(&vec3_j->head, 2);
		transform.position = glm::vec3(x->value.float_value, y->value.float_value, z->value.float_value);
		vec3_j = (JSON_array_t*) JSON_object_get(&transform_j->head, "scale");
		x = (JSON_POD_t*) JSON_array_get(&vec3_j->head, 0);
		y = (JSON_POD_t*) JSON_array_get(&vec3_j->head, 1);
		z = (JSON_POD_t*) JSON_array_get(&vec3_j->head, 2);
		transform.scale = glm::vec3(x->value.float_value, y->value.float_value, z->value.float_value);

		JSON_object_t* renderer_j = (JSON_object_t*) JSON_object_get(&prop->head, "renderer");
		JSON_string_t* mesh = (JSON_string_t*) JSON_object_get(&renderer_j->head, "mesh");
		JSON_string_t* texture = (JSON_string_t*) JSON_object_get(&renderer_j->head, "texture");
		JSON_string_t* shader = (JSON_string_t*) JSON_object_get(&renderer_j->head, "shader");
		renderer_t renderer;
		renderer.mesh = bank.meshes[mesh->value];
		renderer.texture = bank.textures[texture->value];
		renderer.shader = bank.shaders[shader->value];

		scene_add(scene, transform, renderer);
	}
}

void roster_init_RE(std::map<std::string, scene_t>& roster, std::string path, bank_t& bank)
{
	std::cerr << "[roster_init] loading scenes in directory: " << path << std::endl;
	DIR* dir = opendir(path.c_str());
	if(dir == NULL)
	{
		perror("opendir");
		exit(EXIT_FAILURE);
	}
	struct dirent* ent;

	while((ent = readdir(dir)) != NULL)
	{
		if(strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0)
		{
			char subpath[512];
			snprintf(subpath, 512, "%s/%s", path.c_str(), ent->d_name);

			struct stat st;
			if(stat(subpath, &st) != 0)
			{
				perror("stat");
				exit(EXIT_FAILURE);
			}

			if(S_ISDIR(st.st_mode))
			{
				roster_init_RE(roster, subpath, bank);
			}
			else
			{
				std::string subpath_str = std::string(subpath);
				int slash = subpath_str.rfind('/');
				int dot = subpath_str.rfind('.');
				int end = subpath_str.size();
				std::string stem = subpath_str.substr(0, slash);
				std::string name = subpath_str.substr(slash+1, dot-slash-1);
				std::string ext = subpath_str.substr(dot+1, end-dot-1);

				if(ext == "scene")
				{
					std::cerr << "[roster_init] loading scene: " << subpath_str << std::endl;
					JSON_head_t* json = JSON_read(subpath_str.c_str());
					scene_JSON_init(roster[name], json, bank);
					JSON_dispose(json);
				}
			}
		}
	}

	closedir(dir);
}

void roster_init(std::map<std::string, scene_t>& roster, std::string path, bank_t& bank)
{
	roster = std::map<std::string, scene_t>();
	roster_init_RE(roster, path, bank);
}

