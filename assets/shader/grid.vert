#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 norm;

uniform mat4 V;
uniform mat4 P;

uniform float near;
uniform float far;

out vert_out
{
	vec3 near_pos;
	vec3 far_pos;
} o;

vec3 unproject(float x, float y, float z) 
{
    vec4 pos = inverse(V) * inverse(P) * vec4(x, y, z, 1.0);
    return pos.xyz / pos.w;
}

void main()
{
	o.near_pos = unproject(pos.x, pos.y, -1.0);
	o.far_pos = unproject(pos.x, pos.y, 1.0);
	gl_Position = vec4(pos, 1.0);
}
