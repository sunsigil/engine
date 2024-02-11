#version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 norm;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

out vert_out
{
	vec4 pos;
	vec2 uv;
	vec4 norm;
} o;

void main()
{
	o.pos = P*V*M * vec4(pos, 1);
	o.uv = uv;
	o.norm = normalize(transpose(inverse(M)) * vec4(norm, 0));
	gl_Position = o.pos;
}
