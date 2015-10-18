#version 330

in vec3 iPosition;
in vec2 iUV;

uniform mat4 uMvp;

out vec2 vUV;

void main()
{
	gl_Position = uMvp * vec4(iPosition, 1);
	vUV = iUV;
}