#pragma once

#include <vector>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtx/projection.hpp>

#include "geometry.hpp"
#include "mesh.hpp"

vertex_t* gen_AA_plane(axis_t axis, orient_t orient)
{
	glm::vec3 n = axis_normal(axis, orient); 
	int axis_a = (axis+1)%3;
	int axis_b = (axis+2)%3;

	std::vector<vertex_t> verts(4);
	for(int i = 0; i < 4; i++)
	{
		verts[i].pos[axis] = 0;
		verts[i].norm = n;
	}
	verts[0].pos[axis_a] = -1; verts[0].pos[axis_b] = -1;
	verts[0].uv = glm::vec2(0,0);
	verts[1].pos[axis_a] = -1; verts[1].pos[axis_b] = 1;
	verts[1].uv = glm::vec2(0,1);
	verts[2].pos[axis_a] = 1; verts[2].pos[axis_b] = -1;
	verts[2].uv = glm::vec2(1,0);
	verts[3].pos[axis_a] = 1; verts[3].pos[axis_b] = 1;
	verts[3].uv = glm::vec2(1,1);

	std::vector<vertex_t> tris(6);
	tris[0] = verts[0]; tris[1] = verts[1]; tris[2] = verts[3];
	tris[3] = verts[3]; tris[4] = verts[2]; tris[5] = verts[0];
	if(orient == CW)
	{
		std::swap(tris[1], tris[2]);
		std::swap(tris[4], tris[5]);
	}

	vertex_t* mesh = new vertex_t[6];
	copy(tris.begin(), tris.end(), mesh);
	return mesh;
}

vertex_t* gen_AA_box()
{
	vertex_t* mesh = new vertex_t[36];
	int idx = 0;
	for(int axis = X; axis <= Z; axis += 1)
	{
		for(int orient = CW; orient <= CCW; orient += 1)
		{
			vertex_t* plane = gen_AA_plane((axis_t) axis, (orient_t) orient);
			for(int i = 0; i < 6; i++)
			{
				plane[i].pos += plane[i].norm;
				mesh[idx+i] = plane[i];
			}
			idx += 6;
			delete[] plane;
		}
	}

	return mesh;
}

