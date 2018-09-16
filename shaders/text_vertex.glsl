#version 140

in vec2 in_position;
in vec2 in_uv;

out vec2 uv;

void main()
{
	gl_Position = vec4(in_position * 2 - 1, 0, 1);

    uv = in_uv;
}