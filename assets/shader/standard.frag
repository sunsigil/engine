#version 330

uniform sampler2D tex;

in vert_out
{
	vec4 pos;
	vec2 uv;
	vec4 norm;
} i;

out vec4 o;

void main()
{
	o = texture(tex, i.uv);
}

