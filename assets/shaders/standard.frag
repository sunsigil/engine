#version 330

in vert_out
{
	vec4 pos;
	vec2 uv;
	vec4 norm;
	vec4 pos_world;
	vec4 pos_light;
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
uniform sampler2D shadow_map; 

out vec4 o;

float shadowing(vec4 pos_light)
{
	vec3 pos_ndc = pos_light.xyz / pos_light.w;
	vec3 pos_normalized = pos_ndc * 0.5 + 0.5;
	float near_shadow = texture(shadow_map, pos_normalized.xy).r;
	return near_shadow < pos_normalized.z ? 1.0 : 0.0;
}

void main()
{
	o = Ka;

	for(int lidx = 0; lidx < 1; lidx++)
	{
		if(shadowing(i.pos_light) < 1)
		{
			vec4 light = lights[lidx];
			vec4 light_vec = light - i.pos_world * light.w;
			float light_dist = length(light_vec);
			float atten = light.w == 1 ? 1.0f/light_dist : 1;

			vec4 light_dir = light / light_dist;
			float lambert = clamp(dot(i.norm, light_dir), 0, 1);
			o += texture(map_Kd, i.uv) * lambert * atten;

			vec4 view = normalize(eye - i.pos_world);
			vec4 halfway = normalize(view + light_dir);
			float highlight = pow(max(dot(i.norm, halfway), 0), Ns);
			o += Ks * highlight * atten;
		}
	}
}
