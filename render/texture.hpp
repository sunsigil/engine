#pragma once

#include <stdlib.h>

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <GL/glew.h> 

#include "error.hpp"

struct texture_t
{
	int width;
	int height;

	GLuint tex_id;
	GLubyte* data;
	GLenum mode; 
	GLenum format;

	texture_t()
	{
		this->width = 0;
		this->height = 0;
		this->tex_id = -1;
		this->data = nullptr;
		this->mode = GL_TEXTURE_2D;
		this->format = GL_RGB;
	}

	texture_t(GLubyte* data, int width, int height, GLenum format)
	{
		GLuint tex_id; 
		glGenTextures(1, &tex_id);
		glBindTexture(GL_TEXTURE_2D, tex_id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		this->width = width;
		this->height = height;
		this->tex_id = tex_id;
		this->data = data;
		this->mode = GL_TEXTURE_2D;
		this->format = format;
	}

	~texture_t()
	{
		glDeleteTextures(1, &this->tex_id);
	 	delete[] this->data;
	}
};

