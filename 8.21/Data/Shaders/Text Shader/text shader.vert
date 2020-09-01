#version 450 core

layout (location = 0) in vec2 vertex;

uniform mat4 projection;
uniform mat4 view;

uniform float text_width;
uniform float text_height;

uniform float scale;
uniform vec2 position;

out vec2 out_vertex;

void main() {
	float x = vertex.x * text_width * scale + position.x;
	float y = vertex.y * text_height * scale + position.y;
	gl_Position = projection * vec4(vec2(x, y), 0, 1);

	out_vertex = vec2(vertex.x, -vertex.y);
}
