#version 450 core

layout (location = 0) out vec3 f_color;

in vec2 out_vertex;
in vec2 out_uv;
in float out_height;

uniform sampler2D tile_texture;

void main() {
	//f_color = vec3(1.0, 0, 0);
	f_color = texture(tile_texture, out_uv).xyz;
	f_color = f_color * ((out_height * 0.5) + 1);
	//f_color = vec3(out_uv.x, out_uv.y, 1);
}