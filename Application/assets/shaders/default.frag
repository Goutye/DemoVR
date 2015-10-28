#version 330

in vec3 vNormal;
in vec2 vUV;

uniform sampler2D uSampler;

out vec4 oColor;

void main()
{	
	oColor = texture(uSampler, vUV.st);
	float a = oColor.r + oColor.g + oColor.b;
	a /= 3;
	oColor = vec4(a, a, a, 1);
}
