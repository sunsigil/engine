#version 330

in vert_out
{
	vec4 pos;
	vec2 uv;
	vec4 norm;
	vec4 pos_world;
} i;

uniform vec4 Ka;
uniform vec4 Kd;
uniform vec4 Ks;
uniform float d;
uniform bool halo;
uniform int Ns;
uniform sampler2D map_Ka;
uniform sampler2D map_Kd;
uniform sampler2D map_Ks;
uniform sampler2D map_d;

uniform vec4 eye;

uniform vec4 lights[2];

out vec4 o;

void main()
{
	o = Ka;

	for(int lidx = 0; lidx < lights.length(); lidx++)
	{
		vec4 light_dir = normalize(lights[lidx] - i.pos_world);
		float lambert = max(dot(i.norm, light_dir), 0);
		o += texture(map_Kd, i.uv) * lambert;

		vec4 view = normalize(eye - i.pos_world);
		vec4 halfway = normalize(view + light_dir);
		float highlight = pow(max(dot(i.norm, halfway), 0), Ns);
		o += Ks * highlight;
	}
}
