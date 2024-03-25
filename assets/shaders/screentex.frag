#version 330

uniform sampler2D screen_tex;

in vec2 screen_uv;
out vec4 colour;

void main()
{
	colour = texture(screen_tex, screen_uv);
}
