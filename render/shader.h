#pragma once

#include <string>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

struct shader_t
{
	GLuint vert_id;
	GLuint frag_id;
	GLuint prog_id;
};

shader_t shader_read(char* vert_src, char* frag_src)
{
	int shader_status;
	char shader_log[512];

	unsigned int vert_id = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert_id, 1, &vert_src, NULL);
	glCompileShader(vert_id);
	free(vert_src);
	
	std::cerr << "[shader_read] compiling vertex program " << std::endl;
	glGetShaderiv(vert_id, GL_COMPILE_STATUS, &shader_status);
	if(!shader_status)
	{
		glGetShaderInfoLog(vert_id, 512, NULL, shader_log);
		std::cerr << "[shader_read] error: failed to compile vertex shader " << std::endl;
		std::cerr << shader_log << std::endl;
	}

	unsigned int frag_id = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag_id, 1, &frag_src, NULL);
	glCompileShader(frag_id);
	free(frag_src);

	std::cerr << "[shader_read] compiling fragment program " << std::endl;
	glGetShaderiv(frag_id, GL_COMPILE_STATUS, &shader_status);
	if(!shader_status)
	{
		glGetShaderInfoLog(frag_id, 512, NULL, shader_log);
		std::cerr << "[shader_read] error: failed to compile fragment shader " << std::endl;
		std::cerr << shader_log << std::endl;
	}

	unsigned int prog_id = glCreateProgram();
	glAttachShader(prog_id, vert_id);
	glAttachShader(prog_id, frag_id);
	glLinkProgram(prog_id);
	
	glGetShaderiv(prog_id, GL_LINK_STATUS, &shader_status);
	if(!shader_status)
	{
		glGetShaderInfoLog(prog_id, 512, NULL, shader_log);
		std::cerr << shader_log << std::endl;
	}

	shader_t shader;
	shader.vert_id = vert_id;
	shader.frag_id = frag_id;
	shader.prog_id = prog_id;
	return shader;
}

void shader_dispose(const shader_t& shader)
{
	glDeleteShader(shader.frag_id);
	glDeleteShader(shader.vert_id);
}

