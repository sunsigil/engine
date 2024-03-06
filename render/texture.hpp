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
};

void texture_init(texture_t& texture, GLubyte* data, int width, int height, GLenum format)
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

	texture.width = width;
	texture.height = height;
	texture.tex_id = tex_id;
	texture.data = data;
	texture.mode = GL_TEXTURE_2D;
	texture.format = format;
}

void texture_dispose(texture_t& texture)
{
	glDeleteTextures(1, &texture.tex_id);
	free(texture.data);
}

