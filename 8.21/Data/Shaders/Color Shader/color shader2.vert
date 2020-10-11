#version 450 core

layout (location = 0) in vec3 vertex;

uniform mat4 projection;
uniform mat4 view;

uniform float length;
uniform float height;
uniform float width;
uniform vec3 position;

void main() {
	vec3 f_vertex = vec3(vertex.x * width + position.x, vertex.y * height + position.y, vertex.z * length + position.z);
	gl_Position = projection * view * vec4(f_vertex, 1.0);
}