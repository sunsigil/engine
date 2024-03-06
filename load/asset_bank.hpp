#pragma once

#include "formats/bmp.h"
#include "formats/obj.h"
#include "formats/txt.h"
#include "formats/mtl.h"

#include "texture.hpp"
#include "mesh.hpp"
#include "shader.hpp"
#include "material.hpp"

#include "directory.hpp"
#include "json_basics.hpp"

#include <string>
#include <map>
#include <vector>

struct asset_bank_t
{
	std::map<std::string, texture_t> textures;
	std::map<std::string, mesh_t> meshes;
	std::map<std::string, shader_t> shaders;
	std::map<std::string, material_t> materials;
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
	vertex_t* verts = new vertex_t[vert_count];
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

void material_MTL_init(material_t& material, MTL_t& mtl, asset_bank_t& bank)
{
	material_init(material);
	if(mtl.Ka != NULL)
		material.Ka = glm::vec3(mtl.Ka[0], mtl.Ka[1], mtl.Ka[2]);
	if(mtl.Kd != NULL)
		material.Kd = glm::vec3(mtl.Kd[0], mtl.Kd[1], mtl.Kd[2]);
	if(mtl.Ks != NULL)
		material.Ks = glm::vec3(mtl.Ks[0], mtl.Ks[1], mtl.Ks[2]);
	material.d = mtl.d;
	material.halo = mtl.halo;
	material.Ns = mtl.Ns;
	if(mtl.map_Ka != NULL)
		material.map_Ka = &bank.textures[mtl.map_Ka];
	if(mtl.map_Kd != NULL)
		material.map_Kd = &bank.textures[mtl.map_Kd];
	if(mtl.map_Ks != NULL)
		material.map_Ks = &bank.textures[mtl.map_Ks];
	if(mtl.map_d != NULL)
		material.map_d = &bank.textures[mtl.map_d];
}

void asset_bank_init(asset_bank_t& bank, dir_node_t* dir)
{
	bank.textures = std::map<std::string, texture_t>();
	bank.meshes = std::map<std::string, mesh_t>();
	bank.shaders = std::map<std::string, shader_t>();
	bank.materials = std::map<std::string, material_t>();

	std::vector<dir_node_t*> files = std::vector<dir_node_t*>();
	dir_pick_leaves(dir, files);

	for(int i = 0; i < files.size(); i++)
	{
		dir_node_t* file = files[i];
		if(file->extension == "bmp")
		{
			std::cerr << "[asset_bank_init] loading texture asset: " << file->path << std::endl;
			BMP_t bmp;
			BMP_read(&bmp, file->path.c_str());
			texture_BMP_init(bank.textures[file->name], bmp);
			BMP_dispose(&bmp);
		}
		else if(file->extension == "obj")
		{
			std::cerr << "[asset_bank_init] loading mesh asset: " << file->path << std::endl;
			OBJ_t obj;
			OBJ_init(&obj, file->path.c_str());
			mesh_OBJ_init(bank.meshes[file->name], obj);
			OBJ_dispose(&obj);
		}
		else if(file->extension == "vert")
		{
			std::string vert_path = file->stem + "/" + file->name + ".vert";
			std::string frag_path = file->stem + "/" + file->name + ".frag";
			std::cerr << "[asset_bank_init] loading shader asset: " << vert_path << ", " << frag_path << std::endl;
			TXT_t vert, frag;
			TXT_init(&vert, vert_path.c_str());
			TXT_init(&frag, frag_path.c_str());
			shader_TXT_init(bank.shaders[file->name], vert, frag);
			TXT_dispose(&vert);
			TXT_dispose(&frag);
		}
	}
	for(int i = 0; i < files.size(); i++)
	{
		dir_node_t* file = files[i];
		if(file->extension == "mtl")
		{
			std::cerr << "[asset_bank_init] loading material asset: " << file->path << std::endl;
			MTL_t mtl;
			MTL_init(&mtl, file->path.c_str());
			material_MTL_init(bank.materials[file->name], mtl, bank);
			MTL_dispose(&mtl);
		}
	}
}

