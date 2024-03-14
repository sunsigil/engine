#pragma once

#include <string>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "error.hpp"

struct shader_t
{
	GLuint vert_id;
	GLuint frag_id;
	GLuint prog_id;

	shader_t(std::string vert_src, std::string frag_src)
	{
		int shader_status;
		char shader_log[512];

		std::cerr << "[shader_t] compiling vertex program " << std::endl;
		unsigned int vert_id = glCreateShader(GL_VERTEX_SHADER);
		const char* vert_src_cstr = vert_src.c_str();
		glShaderSource(vert_id, 1, &vert_src_cstr, NULL);
		glCompileShader(vert_id);
		
		glGetShaderiv(vert_id, GL_COMPILE_STATUS, &shader_status);
		if(shader_status != GL_TRUE)
		{
			glGetShaderInfoLog(vert_id, 512, NULL, shader_log);
			std::cerr << "[shader_t] error: failed to compile vertex shader " << std::endl;
			std::cerr << shader_log << std::endl;
		}

		std::cerr << "[shader_t] compiling fragment program " << std::endl;
		unsigned int frag_id = glCreateShader(GL_FRAGMENT_SHADER);
		const char* frag_src_cstr = frag_src.c_str();
		glShaderSource(frag_id, 1, &frag_src_cstr, NULL);
		glCompileShader(frag_id);

		glGetShaderiv(frag_id, GL_COMPILE_STATUS, &shader_status);
		if(shader_status != GL_TRUE)
		{
			glGetShaderInfoLog(frag_id, 512, NULL, shader_log);
			std::cerr << "[shader_t] error: failed to compile fragment shader " << std::endl;
			std::cerr << shader_log << std::endl;
		}

		std::cerr << "[shader_t] linking shader program " << std::endl;
		unsigned int prog_id = glCreateProgram();
		glAttachShader(prog_id, vert_id);
		glAttachShader(prog_id, frag_id);
		glLinkProgram(prog_id);
		
		glGetProgramiv(prog_id, GL_LINK_STATUS, &shader_status);
		if(shader_status != GL_TRUE)
		{
			glGetShaderInfoLog(prog_id, 512, NULL, shader_log);
			std::cerr << "[shader_t] error: failed to link shader program" << std::endl;
			std::cerr << shader_log << std::endl;
		}

		this->vert_id = vert_id;
		this->frag_id = frag_id;
		this->prog_id = prog_id;
	}

	~shader_t()
	{
		glDeleteShader(this->vert_id);
		glDeleteShader(this->frag_id);
		glDeleteProgram(this->prog_id);
	}
};

