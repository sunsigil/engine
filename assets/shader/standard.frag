#version 330

in vert_out
{
	vec4 pos;
	vec2 uv;
	vec4 norm;
} i;

uniform sampler2D map_Kd;

out vec4 o;

void main()
{
	vec4 light_dir = vec4(1,1,1,0);

	vec4 c = texture(map_Kd, i.uv);

	vec3 n = normalize(i.norm.xyz);
	vec3 l = -normalize(light_dir.xyz);
	// vec3 v = normalize((cam_pos - i.world_pos).xyz);
	// vec3 h = normalize(l + v);
	// float nh = clamp(dot(n, h), 0, 1);
	float nl = clamp(dot(i.norm.xyz, light_dir.xyz), 0, 1);
	// float spec = pow(max(dot(n, h), 0), 50);

	// (ka * od) + (nl * kd * od) + (dot(n, h)^n * ks * os)
	o = 0.2 * c + nl * c; // + 0.5 * spec * vec4(1);
	o.a = 1;
}
