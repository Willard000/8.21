#version 450 core

layout (location = 0) in vec2 vertex;

uniform mat4 projection;
uniform mat4 view;

uniform vec2 position;
uniform float width;
uniform float height;

void main() {
	vec2 f_vertex = vec2(vertex.x * width + position.x, vertex.y * height + position.y);
	gl_Position = projection * vec4(f_vertex, 0, 1);
}
