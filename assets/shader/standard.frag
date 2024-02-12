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
	vec4 clr = texture(tex, i.uv);
	float amb = 0.15;
	float diff = clamp(-dot(i.norm.xyz, vec3(1)), 0, 1);

	o = clr * amb + clr * diff;
}
