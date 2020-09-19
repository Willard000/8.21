#version 450 core

layout (location = 0) out vec4 f_color;

in vec2 out_uv;

uniform vec4 screen_space;
uniform int highlight;
uniform sampler2D icon_texture;

void main() {
	if(screen_space.x > 0 || screen_space.y > 0) {
		if(gl_FragCoord.y < screen_space.y || gl_FragCoord.y > screen_space.y + screen_space.w ||
 	   	gl_FragCoord.x < screen_space.x || gl_FragCoord.x > screen_space.x + screen_space.z) {
			discard;
		}
	}

	f_color = texture(icon_texture, out_uv);

	if(highlight == 0 && f_color == vec4(0, 0, 0, 1)) {
		discard;
	}

	
	//f_color = vec4(1, 0, 0, 1);
}