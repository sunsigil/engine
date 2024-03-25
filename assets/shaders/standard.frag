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
	float shadow = 0.0f;
	vec2 texel_size = 1.0f / textureSize(shadow_map, 0);
	for(int y = -1; y <= 1; y++)
	{
		for(int x = -1; x <= 1; x++)
		{
			float pcf = texture(shadow_map, pos_normalized.xy + vec2(x,y)*texel_size).r;
			shadow += float(pcf < pos_normalized.z - 0.005);
		}
	}
	return shadow/9;
}

void main()
{
	o = Ka;

	for(int lidx = 0; lidx < 2; lidx++)
	{
		float light_strength = 1-shadowing(i.pos_light) + float(lights[lidx].w > 0);
		light_strength = clamp(light_strength, 0, 1);
		vec4 light = vec4(0,0,0,1);

		vec4 light_vec = lights[lidx];
		vec4 light_ray = light_vec - i.pos_world * light_vec.w;
		float light_dist = length(light_ray);
		float atten = light_vec.w == 1 ? 1.0f/light_dist : 1;

		vec4 light_dir = light_ray / light_dist;
		float lambert = clamp(dot(i.norm, light_dir), 0, 1);
		light += texture(map_Kd, i.uv) * lambert * atten;

		vec4 view = normalize(eye - i.pos_world);
		vec4 halfway = normalize(view + light_dir);
		float highlight = pow(max(dot(i.norm, halfway), 0), Ns);
		light += Ks * highlight * atten;

		o += light_strength * light;
	}
}
