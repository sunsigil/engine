#pragma once

#include "formats/bmp.h"
#include "formats/obj.h"
#include "formats/txt.h"

#include "texture.hpp"
#include "mesh.hpp"
#include "shader.hpp"

#include <string>
#include <map>
#include <dirent.h>
#include <sys/stat.h>

struct bank_t
{
	std::map<std::string, texture_t> textures;
	std::map<std::string, mesh_t> meshes;
	std::map<std::string, shader_t> shaders;
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

void bank_init_RE(bank_t& bank, std::string path)
{
	std::cerr << "[bank_init] loading assets in directory: " << path << std::endl;
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
				bank_init_RE(bank, subpath);
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

				if(ext == "bmp" || ext == "BMP")
				{
					std::cerr << "[bank_init] loading texture asset: " << subpath_str << std::endl;
					BMP_t bmp;
					BMP_read(&bmp, subpath);
					texture_BMP_init(bank.textures[name], bmp);
					BMP_dispose(&bmp);
				}
				else if(ext == "obj" || ext == "OBJ")
				{
					std::cerr << "[bank_init] loading mesh asset: " << subpath_str << std::endl;
					OBJ_t obj;
					OBJ_init(&obj, subpath);
					mesh_OBJ_init(bank.meshes[name], obj);
					OBJ_dispose(&obj);
				}
				else if(ext == "vert")
				{
					std::string vert_path = stem + "/" + name + ".vert";
					std::string frag_path = stem + "/" + name + ".frag";
					std::cerr << "[bank_init] loading shader asset: " << vert_path << ", " << frag_path << std::endl;
					TXT_t vert, frag;
					TXT_init(&vert, vert_path.c_str());
					TXT_init(&frag, frag_path.c_str());
					shader_TXT_init(bank.shaders[name], vert, frag);
					TXT_dispose(&vert);
					TXT_dispose(&frag);
				}
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
	bank_init_RE(bank, path);
}

