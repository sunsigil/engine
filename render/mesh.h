#pragma once

#include <GL/glew.h> 
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtx/string_cast.hpp>

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
};

mesh_t mesh_init(vertex_t* verts, int vert_count, GLenum mode)
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

	mesh_t mesh;
	mesh.vao_id = vao_id;
	mesh.vbo_id = vbo_id;
	mesh.verts = verts;
	mesh.mode = mode;

	return mesh;
}

void mesh_dispose(const mesh_t& mesh)
{
	if(mesh.vert_count == 0)
	{ return; }

	glDeleteVertexArrays(1, &mesh.vao_id);
	glDeleteBuffers(1, &mesh.vbo_id);
}
