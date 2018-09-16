#version 140

in vec2 uv;

uniform sampler2D sampler;

uniform vec4 color;

void main()
{
    gl_FragColor = color * texture( sampler, uv );
}