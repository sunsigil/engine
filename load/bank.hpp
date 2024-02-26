#pragma once

#include "formats/bmp.h"
#include "formats/obj.h"
#include "formats/txt.h"
#include "formats/json.h"
#include "formats/mtl.h"

#include "texture.hpp"
#include "mesh.hpp"
#include "shader.hpp"
#include "material.hpp"
#include "scene.hpp"

#include "json_basics.hpp"

#include <string>
#include <map>
#include <vector>
#include <dirent.h>
#include <sys/stat.h>

struct asset_file_t
{
	std::string path;
	std::string stem;
	std::string name;
	std::string ext;
};

struct bank_t
{
	std::map<std::string, texture_t> textures;
	std::map<std::string, mesh_t> meshes;
	std::map<std::string, shader_t> shaders;
	std::map<std::string, material_t> materials;
	std::map<std::string, scene_t> scenes;
};

void texture_BMP_init(texture_t& texture, BMP_t& bmp)
{
	GLubyte* array_copy = (GLubyte*) malloc(bmp.array_size);
	memcpy(array_copy, bmp.array, bmp.array_size);

	int channels = bmp.depth/8;
	GLenum format = channels == 3 ? GL_RGB : GL_RGBA;

	texture_init(texture, array_copy, bmp.width, bmp.height, format);
}

void mesh_OBJ_init(mesh_t& mesh, OBJ_t& obj)
{
	int vert_count = obj.f_count * 3;
	vertex_t* verts = (vertex_t*) calloc(vert_count, sizeof(vertex_t));
	int vert_idx = 0;
	for(int f_idx = 0; f_idx < obj.f_count; f_idx++)
	{
		int* f = obj.fs+f_idx*OBJ_F_STRIDE;
		for(int p_idx = 0; p_idx < OBJ_F_POINT_COUNT; p_idx++)
		{
			int* p = f+p_idx*OBJ_F_POINT_STRIDE;
			int v_idx = p[0]-1;
			int vt_idx = p[1]-1;
			int vn_idx = p[2]-1;

			vertex_t vert;
			float* v = obj.vs+v_idx*OBJ_V_STRIDE;
			vert.pos = glm::vec3(v[0], v[1], v[2]);
			vert.uv = glm::vec2(0, 0);
			vert.norm = glm::vec3(0,1,0);
			if(vt_idx >= 0)
			{
				float* vt = obj.vts+vt_idx*OBJ_VT_STRIDE;
				vert.uv = glm::vec2(vt[0], vt[1]);
			}
			if(vn_idx >= 0)
			{
				float* vn = obj.vns+vn_idx*OBJ_VN_STRIDE;
				vert.norm = glm::vec3(vn[0], vn[1], vn[2]);
			}
			
			verts[vert_idx] = vert;
			vert_idx += 1;
		}
	}
	
	mesh_init(mesh, verts, vert_count, GL_TRIANGLES);
}

void shader_TXT_init(shader_t& shader, TXT_t& vert_txt, TXT_t& frag_txt)
{
	shader_init(shader, vert_txt.text, frag_txt.text);	
}

void material_MTL_init(material_t& material, MTL_t& mtl, bank_t& bank)
{
	material_init(material);
	material.Ka = glm::vec3(mtl.Ka[0], mtl.Ka[1], mtl.Ka[2]);
	material.Kd = glm::vec3(mtl.Kd[0], mtl.Kd[1], mtl.Kd[2]);
	material.Ks = glm::vec3(mtl.Ks[0], mtl.Ks[1], mtl.Ks[2]);
	material.d = mtl.d;
	material.halo = mtl.halo;
	material.Ns = mtl.Ns;
	material.map_Ka = &bank.textures[mtl.map_Ka];
	material.map_Kd = &bank.textures[mtl.map_Kd];
	material.map_Ks = &bank.textures[mtl.map_Ks];
	material.map_d = &bank.textures[mtl.map_d];
}

void transform_JSON_init(transform_t& transform, JSON_head_t* json)
{
	if(*json != JSON_OBJECT)
	{
		std::cerr << "[transform_JSON_init] error: expected JSON_OBJECT representation" << std::endl;
		return;
	}

	glm::vec3 scale = vec_JSON_init<3>(JSON_object_get(json, "scale"));
	glm::quat orientation = glm::quat(vec_JSON_init<3>(JSON_object_get(json, "orientation")));
	glm::vec3 position = vec_JSON_init<3>(JSON_object_get(json, "position"));

	transform_init(transform, scale, orientation, position);
}

void renderer_JSON_init(renderer_t& renderer, JSON_head_t* json, bank_t& bank)
{
	if(*json != JSON_OBJECT)
	{
		std::cerr << "[renderer_JSON_init] error: expected JSON_OBJECT representation" << std::endl;
		return;
	}

	JSON_string_t* mesh_name = (JSON_string_t*) JSON_object_get(json, "mesh");
	mesh_t* mesh = &bank.meshes[mesh_name->value];
	JSON_string_t* shader_name = (JSON_string_t*) JSON_object_get(json, "shader");
	shader_t* shader = &bank.shaders[shader_name->value];
	JSON_string_t* material_name = (JSON_string_t*) JSON_object_get(json, "material");
	material_t* material = &bank.materials[material_name->value];
	
	renderer_init(renderer);
	renderer.mesh = mesh;
	renderer.shader = shader;
	renderer.material = material;
}

void scene_JSON_init(scene_t& scene, JSON_head_t* json, bank_t& bank)
{
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
	scene_init(scene, prop_count+10000);

	for(int i = 0; i < prop_count; i++)
	{
		JSON_head_t* prop = JSON_array_get(props, i);

		JSON_head_t* transform_json = JSON_object_get(prop, "transform");
		transform_t transform;
		transform_JSON_init(transform, transform_json);

		JSON_head_t* renderer_json = JSON_object_get(prop, "renderer");
		renderer_t renderer;
		renderer_JSON_init(renderer, renderer_json, bank);

		scene_add(scene, transform, renderer);
	}
}

void fetch_asset_files(std::string dir_path, std::vector<asset_file_t>& asset_files)
{
	std::cerr << "[fetch_asset_files] searching " << dir_path << std::endl;
	DIR* dir = opendir(dir_path.c_str());
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
			char path[512];
			snprintf(path, 512, "%s/%s", dir_path.c_str(), ent->d_name);

			struct stat st;
			if(stat(path, &st) != 0)
			{
				perror("stat");
				exit(EXIT_FAILURE);
			}

			if(S_ISDIR(st.st_mode))
			{
				fetch_asset_files(path, asset_files);
			}
			else
			{
				std::string path_str = std::string(path);
				int slash = path_str.rfind('/');
				int dot = path_str.rfind('.');
				int end = path_str.size();

				asset_file_t file;
				file.path = path_str;
				file.stem = dir_path;
				file.name = path_str.substr(slash+1, dot-slash-1);
				file.ext = path_str.substr(dot+1, end-dot-1);
				asset_files.push_back(file);
			}
		}
	}

	closedir(dir);
}

void bank_init(bank_t& bank, std::string path)
{
	bank.textures = std::map<std::string, texture_t>();
	bank.meshes = std::map<std::string, mesh_t>();
	bank.shaders = std::map<std::string, shader_t>();
	bank.materials = std::map<std::string, material_t>();
	bank.scenes = std::map<std::string, scene_t>();

	std::vector<asset_file_t> asset_files = std::vector<asset_file_t>();
	fetch_asset_files(path, asset_files);

	for(int i = 0; i < asset_files.size(); i++)
	{
		asset_file_t file = asset_files[i];
		if(file.ext == "bmp")
		{
			std::cerr << "[bank_init] loading texture asset: " << file.path << std::endl;
			BMP_t bmp;
			BMP_read(&bmp, file.path.c_str());
			texture_BMP_init(bank.textures[file.name], bmp);
			BMP_dispose(&bmp);
		}
		else if(file.ext == "obj")
		{
			std::cerr << "[bank_init] loading mesh asset: " << file.path << std::endl;
			OBJ_t obj;
			OBJ_init(&obj, file.path.c_str());
			mesh_OBJ_init(bank.meshes[file.name], obj);
			OBJ_dispose(&obj);
		}
		else if(file.ext == "vert")
		{
			std::string vert_path = file.stem + "/" + file.name + ".vert";
			std::string frag_path = file.stem + "/" + file.name + ".frag";
			std::cerr << "[bank_init] loading shader asset: " << vert_path << ", " << frag_path << std::endl;
			TXT_t vert, frag;
			TXT_init(&vert, vert_path.c_str());
			TXT_init(&frag, frag_path.c_str());
			shader_TXT_init(bank.shaders[file.name], vert, frag);
			TXT_dispose(&vert);
			TXT_dispose(&frag);
		}
	}
	for(int i = 0; i < asset_files.size(); i++)
	{
		asset_file_t file = asset_files[i];
		if(file.ext == "mtl")
		{
			std::cerr << "[bank_init] loading material asset: " << file.path << std::endl;
			MTL_t mtl;
			MTL_init(&mtl, file.path.c_str());
			material_MTL_init(bank.materials[file.name], mtl, bank);
			MTL_dispose(&mtl);
		}
	}
	for(int i = 0; i < asset_files.size(); i++)
	{
		asset_file_t file = asset_files[i];
		if(file.ext == "scene")
		{
			std::cerr << "[bank_init] loading scene asset: " << file.path << std::endl;
			JSON_head_t* json = JSON_read(file.path.c_str());
			scene_JSON_init(bank.scenes[file.name], json, bank);
			JSON_dispose(json);
		}
	}
}

