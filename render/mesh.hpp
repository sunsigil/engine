#pragma once

#include <GL/glew.h> 
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <glm/glm.hpp>

struct vertex_t
{
	glm::vec3 pos;
	glm::vec2 uv;
	glm::vec3 norm;
};

struct mesh_t
{
	GLuint vao_id;
	GLuint vbo_id;
	int vert_count;
	vertex_t* verts;
	GLenum mode;

	mesh_t()
	{
		this->vao_id = -1;
		this->vbo_id = -1;
		this->vert_count = 0;
		this->verts = nullptr;
		this->mode = GL_TRIANGLES;
	}

	mesh_t(vertex_t* verts, int vert_count, GLenum mode)
	{
		GLuint vao_id;
		glGenVertexArrays(1, &vao_id);
		glBindVertexArray(vao_id);

		GLuint vbo_id;
		glGenBuffers(1, &vbo_id);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
		glBufferData(GL_ARRAY_BUFFER, vert_count * sizeof(vertex_t), verts, GL_STATIC_DRAW);
		
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*) offsetof(vertex_t, pos));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*) offsetof(vertex_t, uv));
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*) offsetof(vertex_t, norm));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		this->vao_id = vao_id;
		this->vbo_id = vbo_id;
		this->verts = verts;
		this->vert_count = vert_count;
		this->mode = mode;
	}

	~mesh_t()
	{
		if(this->vert_count == 0)
		{ return; }

		glDeleteVertexArrays(1, &this->vao_id);
		glDeleteBuffers(1, &this->vbo_id);
		delete[] this->verts;
	}
};
