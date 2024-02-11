#pragma once

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <GL/glew.h> 

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "mesh.hpp"
#include "shader.hpp"
#include "texture.hpp"

struct renderer_t
{
	mesh_t mesh;
	shader_t shader;
	texture_t texture;
};

void set_float_uniform(shader_t& shader, std::string name, float value)
{
	GLint loc = glGetUniformLocation(shader.prog_id, name.c_str());
	if(loc != -1)
	{ glProgramUniform1f(shader.prog_id, loc, value); }
}

void set_vec4_uniform(shader_t& shader, std::string name, glm::vec4 value)
{
	GLint loc = glGetUniformLocation(shader.prog_id, name.c_str());
	if(loc != -1)
	{ glProgramUniform4fv(shader.prog_id, loc, 1, glm::value_ptr(value)); }
}

void set_mat4_uniform(shader_t& shader, std::string name, glm::mat4 value)
{
	GLint loc = glGetUniformLocation(shader.prog_id, name.c_str());
	if(loc != -1)
	{ glProgramUniformMatrix4fv(shader.prog_id, loc, 1, GL_FALSE, glm::value_ptr(value)); }
}

void set_tex2_uniform(shader_t& shader, std::string name, int n, texture_t value)
{
	GLint loc = glGetUniformLocation(shader.prog_id, name.c_str());
	if(loc != -1)
	{
		glProgramUniform1i(shader.prog_id, loc, n);
		glActiveTexture(GL_TEXTURE0+n);
		glBindTexture(GL_TEXTURE_2D, value.tex_id);
	}
}

void render(renderer_t& renderer, glm::mat4 M, glm::mat4 V, glm::mat4 P)
{
	glUseProgram(renderer.shader.prog_id);
	glBindVertexArray(renderer.mesh.vao_id);

	set_mat4_uniform(renderer.shader, "M", M);
	set_mat4_uniform(renderer.shader, "V", V);
	set_mat4_uniform(renderer.shader, "P", P);

	set_tex2_uniform(renderer.shader, "tex", 0, renderer.texture);

	glDrawArrays(renderer.mesh.mode, 0, renderer.mesh.vert_count);
	glBindVertexArray(0);
	glUseProgram(0);
}

