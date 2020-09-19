#version 450 core

layout (location = 0) out vec4 f_color;

uniform vec4 color;
uniform vec4 screen_space;

void main() {
	if(screen_space.x > 0 || screen_space.y > 0) {
		if(gl_FragCoord.y < screen_space.y || gl_FragCoord.y > screen_space.y + screen_space.w ||
 	   	gl_FragCoord.x < screen_space.x || gl_FragCoord.x > screen_space.x + screen_space.z) {
			discard;
		}
	}

	f_color = color;
}
