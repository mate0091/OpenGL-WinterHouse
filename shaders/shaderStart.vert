//vertex shader for no lighting
#version 410 core

layout(location=0) in vec3 vPosition;
layout(location=1) in vec3 vNormal;
layout(location=2) in vec2 textcoords;

//matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 texCoord;
out vec3 vNorm;
out vec3 fragPos;

void main()
{
	texCoord = textcoords;
	vNorm = vNormal;
	fragPos = vec3(model * vec4(vPosition, 1.0));
	//transform vertex
	gl_Position = projection * view * model * vec4(vPosition, 1.0f);
}