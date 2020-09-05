#version 450 core

layout (location = 0) out vec4 f_color;

in vec2 out_vertex;

uniform float text_width;
uniform float text_height;
uniform vec2 text_position;

uniform vec4 color;
uniform vec4 screen_space;

uniform sampler2D font_atlas;

void main() {
	if(gl_FragCoord.y < screen_space.y || gl_FragCoord.y > screen_space.y + screen_space.w ||
 	   gl_FragCoord.x < screen_space.x || gl_FragCoord.x > screen_space.x + screen_space.z) {
		discard;
	}

	vec2 p = vec2(out_vertex.x * text_width + text_position.x, out_vertex.y * text_height + text_position.y + text_height);
	vec4 f_texture = texture(font_atlas, p);

	if(f_texture == vec4(0, 0, 0, 1)) {
		discard;
	}

	f_color = f_texture * color;
		
	//f_color = vec4(1, 0, out_vertex.y, 1);
}