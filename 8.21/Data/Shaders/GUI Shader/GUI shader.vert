#version 450 core

layout (location = 0) in vec2 vertex;

uniform mat4 projection;
uniform mat4 view;

uniform vec2 position;

void main() {
	gl_Position = projection * vec4(vertex + position, 0, 1);
}
