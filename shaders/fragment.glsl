#version 140

in vec3 normal;
in vec4 position_vs;

uniform vec4 color;

void main(void)
{
	vec3 lightDir = normalize(vec3(0.0f,1.0f,0.0f));
	float diff = max(dot(lightDir,normalize(normal)),0.0f);
	//gl_FragColor = color * (0.5f + 0.5f * normalize(normal).z);
	gl_FragColor = color * 0.3f + diff * color;
	
}
