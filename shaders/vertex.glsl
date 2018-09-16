#version 150

in vec4 in_position;
in vec3 in_normal;

out vec3 normal;
out vec4 position_vs;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
	position_vs = view * model * in_position;
	gl_Position = projection * position_vs;
	normal = transpose(inverse(mat3(view * model))) * in_normal;
}
