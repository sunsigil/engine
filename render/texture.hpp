#pragma once

#include <stdlib.h>

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <GL/glew.h> 

struct texture_t
{
	int width;
	int height;

	GLuint tex_id;
	GLubyte* data;
	GLenum mode; 
	GLenum channels;
};

void texture_init(texture_t* texture, GLubyte* data, int width, int height, GLenum channels)
{
	GLuint tex_id; 
	glGenTextures(1, &tex_id);
	glBindTexture(GL_TEXTURE_2D, tex_id);
	glTexParameteri(tex_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(tex_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(tex_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(tex_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, channels, width, height, 0, channels, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	texture->width = width;
	texture->height = height;
	texture->tex_id = tex_id;
	texture->data = data;
	texture->mode = GL_TEXTURE_2D;
	texture->channels = channels;
}

void texture_dispose(const texture_t* texture)
{
	glDeleteTextures(1, &texture->tex_id);
	free(texture->data);
}

