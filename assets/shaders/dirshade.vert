#version 330

layout(location = 0) in vec3 pos;

uniform mat4 M;
uniform mat4 L;

void main()
{
	gl_Position = L * M * vec4(pos, 1);
}
