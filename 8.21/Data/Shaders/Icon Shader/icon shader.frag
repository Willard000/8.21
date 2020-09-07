#version 450 core

layout (location = 0) out vec4 f_color;

in vec2 out_uv;

uniform sampler2D icon_texture;

void main() {
	f_color = texture(icon_texture, out_uv);
	
	//f_color = vec4(1, 0, 0, 1);
}