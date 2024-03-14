#version 330 core

uniform mat4 V;
uniform mat4 P;

uniform float near;
uniform float far;

in vert_out
{
	vec3 near_pos;
	vec3 far_pos;
} i;

out vec4 o;

vec4 grid(vec3 frag_pos, float scale)
{
    vec2 coord = frag_pos.xz * scale;
    vec2 derivative = fwidth(coord);
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);
    float min_z = min(derivative.y, 1);
    float min_x = min(derivative.x, 1);
    vec4 color = vec4(0.2, 0.2, 0.2, 1.0 - min(line, 1.0));

    if(frag_pos.z > -0.1 * min_z && frag_pos.z < 0.1 * min_z)
    { color.x = 1.0; }
    if(frag_pos.x > -0.1 * min_x && frag_pos.x < 0.1 * min_x)
	{ color.z = 1.0; }

    return color;
}

float depth(vec3 pos)
{
    vec4 clip_pos = P * V * vec4(pos.xyz, 1.0);
    float ndc_depth = clip_pos.z / clip_pos.w;
	return (ndc_depth * gl_DepthRange.diff + gl_DepthRange.near + gl_DepthRange.far) * 0.5f;
}

void main()
{
	float t = -i.near_pos.y / (i.far_pos.y - i.near_pos.y);
	vec3 frag_pos = i.near_pos + t * (i.far_pos - i.near_pos);

	o = grid(frag_pos, 1);
	gl_FragDepth = depth(frag_pos);
}
