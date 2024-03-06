#pragma once

#include <GL/glew.h> 
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <stdio.h>

void glfw_error_callback(int error, const char* message)
{
	fprintf(stderr, "[glfw_error_callback] %d: %s\n", error, message);
}

const char* gl_get_error(GLenum type)
{
	switch(type)
	{
		case GL_NO_ERROR:
			return "GL_NO_ERROR";
		case GL_INVALID_ENUM:
			return "GL_INVALID_ENUM";
		case GL_INVALID_OPERATION:
			return "GL_INVALID_OPERATION";
		case GL_INVALID_VALUE:
			return "GL_INVALID_VALUE";
		case GL_STACK_UNDERFLOW:
			return "GL_STACK_UNDERFLOW";
		case GL_STACK_OVERFLOW:
			return "GL_STACK_OVERFLOW";
		case GL_OUT_OF_MEMORY:
			return "GL_OUT_OF_MEMORY";
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			return "GL_INVALID_FRAMEBUFFER_OPERATION";
		case GL_CONTEXT_LOST:
			return "GL_CONTEXT_LOST";
		case GL_TABLE_TOO_LARGE:
			return "GL_TABLE_TOO_LARGE";
		default:
			return "INVALID ERROR";
	}
}

bool gl_handle_error()
{
	GLenum error;
	int index = 0;
	while((error = glGetError()) != GL_NO_ERROR)
	{
		fprintf(stderr, "[gl_handle_error]: (%d) %s\n", index, gl_get_error(error));
		index += 1;
	}
	return index > 0;
}

