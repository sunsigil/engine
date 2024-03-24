#version 330

layout(location=0) in vec3 pos; 
layout(location=1) in vec2 uv; 
layout(location=2) in vec3 norm;

out vec2 screen_uv;

void main()
{
	gl_Position = vec4(pos.x, pos.y, 0, 1);
	screen_uv = uv;
}

