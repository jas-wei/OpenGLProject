#version 330 core

in vec4 vertexColor;
in vec2 texCoord;

out vec4 FragColor;

// By default, sampler unit is set to 0
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float mixValue;


void main()
{
	//FragColor = vertexColor;
	FragColor = mix( texture(texture1, texCoord), texture(texture2, texCoord), clamp(mixValue, 0.0, 1.0));
}