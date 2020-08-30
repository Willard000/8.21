#version 450 core

layout (location = 0) out vec4 f_color;

uniform vec4 color;
uniform vec4 viewport;

void main() {
	if(gl_FragCoord.y < viewport.y || gl_FragCoord.y > viewport.y + viewport.w ||
 	   gl_FragCoord.x < viewport.x || gl_FragCoord.x > viewport.x + viewport.z) {
		discard;
	}

	f_color = color;
}
