#pragma once

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <GL/glew.h> 

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "mesh.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "material.hpp"
#include "error.hpp"
#include "camera.hpp"

#include <iostream>

struct renderer_t
{
	mesh_t* mesh;
	shader_t* shader;
	material_t* material;
	
	renderer_t()
	{
		this->mesh = nullptr;
		this->shader = nullptr;
		this->material = nullptr;
	}

	renderer_t(mesh_t* mesh, shader_t* shader, material_t* material)
	{
		this->mesh = mesh;
		this->shader = shader;
		this->material = material;
	}
};

bool set_float_uniform(shader_t* shader, std::string name, float value)
{
	GLint loc = glGetUniformLocation(shader->prog_id, name.c_str());
	if(loc != -1)
	{
		glProgramUniform1f(shader->prog_id, loc, value);
		return true;
	}
	return false;
}

bool set_vec4_uniform(shader_t* shader, std::string name, glm::vec4 value)
{
	GLint loc = glGetUniformLocation(shader->prog_id, name.c_str());
	if(loc != -1)
	{
		glProgramUniform4fv(shader->prog_id, loc, 1, glm::value_ptr(value));
		return true;
	}
	return false;
}

bool set_mat4_uniform(shader_t* shader, std::string name, glm::mat4 value)
{
	GLint loc = glGetUniformLocation(shader->prog_id, name.c_str());
	if(loc != -1)
	{
		glProgramUniformMatrix4fv(shader->prog_id, loc, 1, GL_FALSE, glm::value_ptr(value));
		return true;
	}
	return false;
}

bool set_tex2_uniform(shader_t* shader, std::string name, int n, texture_t* value)
{
	if(value == nullptr)
		return false;

	GLint loc = glGetUniformLocation(shader->prog_id, name.c_str());
	if(loc == -1)
		return false;

	glProgramUniform1i(shader->prog_id, loc, n);
	glActiveTexture(GL_TEXTURE0+n);
	glBindTexture(GL_TEXTURE_2D, value->tex_id);
	return true;
}

bool set_tex2_uniform(shader_t* shader, std::string name, int n, GLuint tex_id)
{
	if(tex_id == -1)
		return false;

	GLint loc = glGetUniformLocation(shader->prog_id, name.c_str());
	if(loc == -1)
		return false;

	glProgramUniform1i(shader->prog_id, loc, n);
	glActiveTexture(GL_TEXTURE0+n);
	glBindTexture(GL_TEXTURE_2D, tex_id);
	return true;
}

bool set_int_uniform(shader_t* shader, std::string name, int value)
{
	GLint loc = glGetUniformLocation(shader->prog_id, name.c_str());
	if(loc != -1)
	{
		glProgramUniform1i(shader->prog_id, loc, value);
		return true;
	}
	return false;
}

void render(camera_t& camera, renderer_t& renderer, transform_t& transform, glm::vec4* lights, glm::mat4 L)
{
	glUseProgram(renderer.shader->prog_id);
	glBindVertexArray(renderer.mesh->vao_id);

	set_mat4_uniform(renderer.shader, "M", transform.make_model());
	set_mat4_uniform(renderer.shader, "V", camera.make_view());
	set_mat4_uniform(renderer.shader, "P", camera.proj);
	set_mat4_uniform(renderer.shader, "P_inv", camera.proj_inv);

	set_vec4_uniform(renderer.shader, "Ka", glm::vec4(renderer.material->Ka, 1));
	set_vec4_uniform(renderer.shader, "Kd", glm::vec4(renderer.material->Kd, 1));
	set_vec4_uniform(renderer.shader, "Ks", glm::vec4(renderer.material->Ks, 1));
	set_float_uniform(renderer.shader, "d", renderer.material->d);
	set_int_uniform(renderer.shader, "halo", renderer.material->halo);
	set_int_uniform(renderer.shader, "Ns", renderer.material->Ns);
	set_tex2_uniform(renderer.shader, "map_Ka", 0, renderer.material->map_Ka);
	set_tex2_uniform(renderer.shader, "map_Kd", 1, renderer.material->map_Kd);
	set_tex2_uniform(renderer.shader, "map_Ks", 2, renderer.material->map_Ks);
	set_tex2_uniform(renderer.shader, "map_d", 3, renderer.material->map_d);
	
	set_vec4_uniform(renderer.shader, "eye", glm::vec4(camera.transform.position, 1));
	set_float_uniform(renderer.shader, "near", camera.near);
	set_float_uniform(renderer.shader, "far", camera.far);

	GLint lights_loc = glGetUniformLocation(renderer.shader->prog_id, "lights");
	glProgramUniform4fv(renderer.shader->prog_id, lights_loc, 2, glm::value_ptr(*lights));
	set_mat4_uniform(renderer.shader, "L", L);
		
	glDrawArrays(renderer.mesh->mode, 0, renderer.mesh->vert_count);
	glBindVertexArray(0);
	glUseProgram(0);
}

