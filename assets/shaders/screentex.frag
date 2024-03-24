#version 330

uniform sampler2D screen_tex;

in vec2 screen_uv;
out vec4 colour;

void main()
{
	vec4 sample = texture(screen_tex, screen_uv);
	float depth = sample.r;
	colour = vec4(vec3(depth), 1);
}
