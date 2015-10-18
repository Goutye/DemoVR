#version 330

in vec2 vUV;

uniform sampler2D uTexture;

out vec4 oColor;

void main()
{
	vec2 t = vUV * 6;
	
	if ((int(floor(t.x) + floor(t.y)) & 1) == 0)
		oColor = vec4(1, 1, 1, 1);
 	else
		oColor = vec4(0.5, 0.5, 0.5, 1);
}
