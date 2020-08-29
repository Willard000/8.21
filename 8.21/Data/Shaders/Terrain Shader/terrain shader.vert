#version 450 core

const int height_indices[6] = {1, 0, 2, 1, 2, 3};
const int back_indices[3] = { 1, 0, 0 };
const int right_indices[3] = { 0, 5, 5 };
const int front_indices[3] = { 5, 4, 4 };
const int left_indices[3] = { 2, 1, 1 };

layout (location = 0) in vec2 vertex;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec2 position;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform samplerBuffer height;

out vec2 out_vertex;
out vec2 out_uv;
out float out_height;

float get_height() {
	const int side = gl_VertexID / 6;
	const int vertex_index = gl_VertexID % 6;
	int side_vertex_index = vertex_index;
	if(side < 4) {
		if(vertex_index == 0 || vertex_index == 1 || vertex_index == 4) {
			if(vertex_index == 4) {
				side_vertex_index = 2;
			}

			switch (side) {
				case 0 :
					side_vertex_index = back_indices[side_vertex_index];
					break;
				case 1:
					side_vertex_index = right_indices[side_vertex_index];
					break;
				case 2:
					side_vertex_index = front_indices[side_vertex_index];
					break;
				case 3:
					side_vertex_index = left_indices[side_vertex_index];
					break;
			}

			return texelFetch(height, height_indices[side_vertex_index] + 4 * gl_InstanceID).r;
		}
		else {
			return 0.0f;		
		}
	}
	
	return texelFetch(height, height_indices[vertex_index] + 4 * gl_InstanceID).r;
}

void main() {
	float y = get_height();
	out_vertex = vertex;
	out_uv = uv;
	out_height = y;
		
	gl_Position = projection * view * vec4(vertex.x + position.x, y, vertex.y + position.y, 1.0);
}